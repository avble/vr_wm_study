#ifndef _ONOFF_H_
#define _ONOFF_H_

#include <stdbool.h>

#define RULE_NAME_MAX 256


typedef enum VruleType
{
    e_SIMPLE_RULE = 0,
    e_TIMER_RULE,
//    e_AWAY_RULE,
}VRuleType;




typedef enum
{
    SCHEDULER_STOP = 1, SCHEDULER_RUN,

} VSchedulerState;



typedef struct ruleInfo
{
    struct ruleInfo *psNext;
    VRuleType   ruleType;
    unsigned int    ruleId;
    unsigned int    time;
    char ruleName[RULE_NAME_MAX];

} VRuleInfo;




typedef struct ruleInfoList{
    VRuleInfo *rule_list;    

    // state variable 
    VSchedulerState state; 
    // for condition variable control 
    pthread_mutex_t     Lock;
    pthread_cond_t      Condition;

    // thread ID     
    pthread_t           ruleThread;
    
}VRuleInfoList; 


void vScheduler_init();
void vScheduler_quit();


// initialize the rule 
void vrule_init();
// add a rule to list 
void vrule_add(VRuleInfo *rule);
// remove a rule from list 
void vrule_remove(char *rule_name);
void vrule_quit();
void vrule_print();



#endif


