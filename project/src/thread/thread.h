#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

typedef struct thread_attr
{
    pthread_t thread;
    size_t stack_size;
    int schedpolicy;
    int priority;
    void *(*thread_func)(void *);
    void *arg;
} thread_attr_t;

int create_and_run_thread(thread_attr_t *attr);

#endif