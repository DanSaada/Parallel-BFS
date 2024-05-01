#include "ThreadPool.h"
#include "TaskQueue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

pthread_mutex_t threadPoolRunningTreadsDecreasingMutex = PTHREAD_MUTEX_INITIALIZER;


void runThreadPool(ThreadPool *tp) {
    /* As long as there are tasks in the queue or running threads... */
    while (!isEmpty(tp->q) || tp->runningThreads > 0) {
        /* If we are not past max number of threads */
        if (tp->runningThreads < tp->maxThreads && !isEmpty(tp->q)) {
            /* Run task */
            TaskData data = pop(tp->q);

            pthread_t thread;
            if (pthread_create(&thread, NULL, data.function, data.args)) {
                perror("pthread_create failed.");
                exit(1);
            } else {
//                increaseRunningThreadCount(&(tp->runningThreads));
//                atomic_fetch_add(&(tp->runningThreads),1);
                (tp->runningThreads)++;
                pthread_detach(thread);
            }
        }
    }
}

void decreaseRunningThreadCount(ThreadPool* pool){
    pthread_mutex_lock(&threadPoolRunningTreadsDecreasingMutex);
//    if(pool->runningThreads > 0){
    --(pool->runningThreads);
//    }
    pthread_mutex_unlock(&threadPoolRunningTreadsDecreasingMutex);
}

void increaseRunningThreadCount(ThreadPool* pool){
    pthread_mutex_lock(&threadPoolRunningTreadsDecreasingMutex);
//    if(pool->runningThreads > 0){
    ++(pool->runningThreads);
//    }
    pthread_mutex_unlock(&threadPoolRunningTreadsDecreasingMutex);
}