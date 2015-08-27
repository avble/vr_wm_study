
#ifndef _VTIMER_H_
#define _VTIMER_H_

#include <pthread.h>
#include <sys/queue.h>

#include "onoff.h"


STAILQ_HEAD(QueueHead, QueueEntry);



typedef struct vtimer
{
    VRuleType rule_type;
    char ruleName[RULE_NAME_MAX];
} VTimer;

typedef VTimer* VTimerP; 

typedef struct vscheduler
{
    VSchedulerState state; 
    
    VTimer *timer_list;
    // for mutual exclusion of timer_list 
    pthread_mutex_t     timer_list_m;

    // for condition variable control 
    pthread_mutex_t     Lock;
    pthread_cond_t      Condition;
    
    pthread_t           TimerThread;

}VScheduler;


struct Queue {
  pthread_mutex_t mutex;
  pthread_cond_t cv;
  pthread_cond_t enq_wait_cv;
  pthread_t           TimerThread;
  VSchedulerState state; 
  
  int enq_waiters;
  int length;
  int limit;
  struct QueueHead queue;
};

struct QueueEntry {
  VTimerP item;
  STAILQ_ENTRY(QueueEntry) entries;
};

struct Queue* queue_init();
int queue_destroy(struct Queue *q);
int queue_empty(struct Queue *q);
int queue_full(struct Queue *q);
int queue_enq(struct Queue *q, VTimerP item);
int queue_length(struct Queue *q);
void queue_limit(struct Queue *q, int limit);
void *queue_deq(struct Queue *q);

#endif

