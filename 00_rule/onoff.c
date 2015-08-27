
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "onoff.h"
#include "vtimer.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif



static struct Queue *gScheduler = NULL;
static VRuleInfoList *gVrule = NULL;


//-----------------------------------------------------------------------------
static VTimerP vRuleCheckUpdate(unsigned* wait)
{
    
    VTimerP vtimer = NULL;
    VRuleInfo *cur  = gVrule->rule_list;
    VRuleInfo *pos = NULL;
    

    unsigned long   interval = 24*60*60;
    unsigned long   offset;
    unsigned long   current = time(0);

//    pthread_mutex_lock(&sTracer.LockUpdateList);

    while(cur != NULL)
    {
//        printf("[Debug] %s %d rule name: %s, rule type: %d \n", __func__, __LINE__, cur->ruleName, cur->ruleType);
        if (!cur->time)
        {
            //printf("[DEBUG] %s %d \n", __func__, __LINE__);
            cur = cur->psNext;
            continue;
            
        }

        if (current + 1 < cur->time)
        {
            //printf("[DEBUG] %s %d \n", __func__, __LINE__);
            offset   = cur->time - current;
            interval = MIN(interval, offset);
        }
        else
        {
            //printf("[DEBUG] %s %d \n", __func__, __LINE__);
            interval = 0;
             pos = cur;
            break;
        }
        //printf("[DEBUG] %s %d \n", __func__, __LINE__);

        cur = cur->psNext;
    }

//    pthread_mutex_unlock(&sTracer.LockUpdateList);

    if (pos != NULL)
    {
        vtimer = (VTimer *)calloc(1, sizeof(VTimer));
        vtimer->rule_type = pos->ruleType;
        strcpy(vtimer->ruleName, pos->ruleName);
    }

    *wait = interval;

    return vtimer;
}


//-----------------------------------------------------------------------------
static void vRuleUpdate(VTimerP vtimer)
{
    if (gScheduler == NULL)
    {
        printf("[Warning] The scheduler has not been initialzed yet \n");
    }

    if (vtimer->rule_type == e_TIMER_RULE)
        vrule_remove(vtimer->ruleName);

    
    queue_enq(gScheduler, vtimer);
    
}



//-----------------------------------------------------------------------------
static void* vRuleThread(void* arg)
{
    VTimerP vtimer; 
    unsigned            wait;
    struct timespec     next = {0,0};

    while (gVrule->state == SCHEDULER_RUN)
    {

        pthread_mutex_lock(&gVrule->Lock);
        
        while ((vtimer = vRuleCheckUpdate(&wait)) == NULL)
        {
            next.tv_sec = time(0) + wait;
            printf("[DEBUG] %s %d waiting: %d \n", __func__, __LINE__, wait);
            pthread_cond_timedwait(&gVrule->Condition, &gVrule->Lock, &next);
        }

        pthread_mutex_unlock(&gVrule->Lock);
        
//        if (vtimer)
//            continue;
        if (vtimer)
            vRuleUpdate(vtimer);

    }

    return NULL;
}


static bool vRuleCreateTask()
{
    if (gVrule == NULL)
        return true;

    if (pthread_create(&gVrule->ruleThread, NULL, &vRuleThread, NULL) != 0)
    {
        printf("CreateTracerTask() failed \n");
        return false;
    }
    
    return true;
}


// initialize the rule 
void vrule_init()
{
   if (gVrule != NULL)
        return;

    
    gVrule = (VRuleInfoList *)calloc(1, sizeof(VRuleInfoList));

    gVrule->state = SCHEDULER_RUN;
    
    pthread_mutex_init(&gVrule->Lock, NULL);
    pthread_cond_init(&gVrule ->Condition, NULL);


    
    gVrule->state = SCHEDULER_RUN;
    
     vRuleCreateTask();

  
}

// Insert at the head of list  
void vrule_add(VRuleInfo *rule)
{
//    printf("%s %d \n", __func__, __LINE__);
    pthread_mutex_lock(&gVrule->Lock);

//    printf("%s %d \n", __func__, __LINE__);
    
    if (gVrule->rule_list == NULL)
    {
        gVrule->rule_list = rule;
        gVrule->rule_list->psNext =NULL;
    }
    else
    {
        rule->psNext = gVrule->rule_list;
        gVrule->rule_list = rule;
    }

    pthread_cond_signal(&gVrule->Condition);
    pthread_mutex_unlock(&gVrule->Lock);
    
}

// Insert at the tail of list 
// deprecated function
#if 0
void vrule_add_tail(VRuleInfo *rule)
{
    pthread_mutex_lock(&gVrule->Lock);

    if (gVrule->rule_list == NULL)
    {
        gVrule->rule_list = rule;
        gVrule->rule_list->psNext =NULL;
    }
    else
    {
        rule->psNext = NULL;
        gVrule->rule_list->psNext= rule;
    }
    
    pthread_cond_signal(&gVrule->Condition);
    pthread_mutex_unlock(&gVrule->Lock);
    
}

#endif 

// remove a rule from list 
void vrule_remove(char *rule_name)
{
    //printf("[DEBUG] %s %d rule name: %s \n", __func__, __LINE__, rule_name);
    VRuleInfo *tmp, *prev;
    tmp = gVrule->rule_list;
    
    while(tmp != NULL)
    {
        if(strcmp(tmp->ruleName, rule_name) == 0)
        {
        //printf("[DEBUG] %s %d rule name: %s \n", __func__, __LINE__, rule_name);
            if(tmp == gVrule->rule_list)
            {
            //printf("[DEBUG] %s %d rule name: %s \n", __func__, __LINE__, rule_name);
                gVrule->rule_list = tmp->psNext;
                free(tmp);
                return;
            }
            else
            {
            //printf("[DEBUG] %s %d rule name: %s \n", __func__, __LINE__, rule_name);
                prev->psNext=tmp->psNext;
                free(tmp);
                return ;
            }
        }
        else
        {
        //printf("[DEBUG] %s %d rule name: %s \n", __func__, __LINE__, rule_name);
            prev=tmp;
            tmp= tmp->psNext;
        }
    }
    return;
}


void vrule_quit()
{
    if (gVrule == NULL)
        return;

    gVrule->state = SCHEDULER_STOP;


    // FIXME: wait until the thread is dead 


    pthread_cond_destroy(&gVrule->Condition);
    pthread_mutex_destroy(&gVrule->Lock);


    VRuleInfo *rule = gVrule->rule_list;
    
    while (rule != NULL)
    {
        gVrule->rule_list = gVrule->rule_list->psNext;
        free(rule);
        rule = gVrule->rule_list;
    }

    free(gVrule);
    gVrule = NULL;

}


void vrule_print()
{
    
    VRuleInfo *cur  = gVrule->rule_list;
    
    while(cur != NULL)
    {
        printf("[Debug] %s %d rule name: %s, rule type: %d \n", __func__, __LINE__, cur->ruleName, cur->ruleType);
        cur = cur->psNext;
    }
}



static void vSchedulerExe(VTimerP vtimer)
{
    printf("[Debug] %s, %d a timmer is triggered rule name: %s \n", __FUNCTION__, __LINE__, vtimer->ruleName);

}


//-----------------------------------------------------------------------------
static void* vScheduler_SchedulerThread(void* arg)
{
    VTimerP vtimer; 

    while (gScheduler->state == SCHEDULER_RUN)
    {
        // dequeue a entry 
//        printf("[Debug] queue len: %d \n", queue_length(gScheduler));
        vtimer = queue_deq(gScheduler);
        // process the entry
        if (vtimer != NULL)
        {
            vSchedulerExe(vtimer);
            free(vtimer);
        }
    }

    return NULL;
}


static bool vScheduler_CreateSchedulerTask()
{
    if (gScheduler == NULL)
        return true;

    if (pthread_create(&gScheduler->TimerThread, NULL, &vScheduler_SchedulerThread, NULL) != 0)
    {
        printf("CreateTracerTask() failed \n");
        return false;
    }
    
    return true;
}



void vScheduler_init()
{

    if (gScheduler != NULL)
        return;

    gScheduler =  queue_init();
    queue_limit(gScheduler, 100);


    gScheduler->state = SCHEDULER_RUN;

    vScheduler_CreateSchedulerTask();
    
}

void vScheduler_quit()
{
    // add a NULL packet & 

    gScheduler->state = SCHEDULER_STOP;

    queue_enq(gScheduler, NULL);
    queue_destroy(gScheduler);

}



#if 0

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
static void sm_DestroyTracerTask()
{
    if (sTracer.Tracer)
    {
        sTracer.State = TS_STOP;

        sleep(1);

        pthread_mutex_lock(&sTracer.Lock);
        pthread_cond_signal(&sTracer.Condition);
        pthread_mutex_unlock(&sTracer.Lock);

        pthread_join(sTracer.TracerThread, NULL);

        sTracer.Tracer = 0;
    }
}

//-----------------------------------------------------------------------------
void SM_FreeTracer()
{
    int     i;

    if (!sTracer.Configured)
        return;

    sm_DestroyTracerTask();

    if (sTracer.UpadeList)
    {
        for (i = 0; i < TRACE_MAX_COUNT; i++)
        {
            if (sTracer.UpadeList[i])
            {
                free(sTracer.UpadeList[i]);
                sTracer.UpadeList[i] = 0;
            }
        }

        free(sTracer.UpadeList);
        sTracer.UpadeList = 0;
    }

    sTracer.Configured = false;

    pthread_cond_destroy(&sTracer.Condition);
    pthread_mutex_destroy(&sTracer.Lock);
}

#endif

//-----------------------------------------------------------------------------

