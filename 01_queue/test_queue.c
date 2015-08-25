#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"

struct Queue *g_pCmdQueue = NULL;
int max_len = 10; 

/* this function is run by the second thread */
void *inc_x(void *x_void_ptr)
{
    int i = 0;  
    for (;1==1;)
    {
        i++;
//        if (queue_length(g_pCmdQueue) < max_len) 
        {
            printf("[Queue][Enq]  \n");
            char *buf = malloc(256);
            sprintf(buf, "hello %d", i);
            queue_enq(g_pCmdQueue, buf);
            printf("[Queue][Enq] %s \n", buf);
        }
        usleep(10*1000);
    }

    return NULL;

}

/* this function is run by the second thread */
void *inc_x1(void *x_void_ptr)
{
    for (;1==1;)
    {
        if (queue_length(g_pCmdQueue) == max_len) 
        {
            queue_deq(g_pCmdQueue);
            printf("[Queue][Deq] \n");
        }
        usleep(1000*1000);
    }

    return NULL;
}



int main(int argc, char *argv[])
{
    g_pCmdQueue = queue_init();
    queue_limit(g_pCmdQueue, max_len);

    int x = 0, y = 0;

    /* show the initial values of x and y */
    printf("x: %d, y: %d\n", x, y);
    
    /* this variable is our reference to the second thread */
    pthread_t inc_x_thread;
    pthread_t inc_x_thread1;
    
    /* create a second thread which executes inc_x(&x) */
    pthread_create(&inc_x_thread, NULL, inc_x, &x);
    pthread_create(&inc_x_thread1, NULL, inc_x1, &x);
   

    printf("y increment finished\n");
    
    /* wait for the second thread to finish */
    if(pthread_join(inc_x_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
 
    fprintf(stderr, "Error creating thread\n");
    return 1;

}
