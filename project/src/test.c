/*
    FILE NAME: test.c
    PURPOSE: Test file
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "deque_array.h"
#include "thread.h"
#include "soft_timer.h"

void timer_handler(union sigval arg) {
    static int count = 0;
    // while (1)
    {
        /* code */
        printf("Timer expired %d\n", count++);
        sleep(1);
    }
}

void* thread_handler(void* arg) {
    printf("Thread started\n");
    soft_timer_attr_t attr = {
        .timer_handler = timer_handler,
        .interval = 1,
        .repeat = 1
    };
    create_and_run_timer(&attr);
    sleep(5);
    delete_timer(&attr);
    printf("Thread ended\n");

    // return NULL;
}

int main() {
    printf("\n\n");
    
    thread_attr_t attr = {
        .stack_size = 1024 * 1024,
        .schedpolicy = SCHED_FIFO,
        .priority = 99,
        .thread_func = thread_handler,
        .arg = NULL
    };
    create_and_run_thread(&attr);

    sleep(10);

    return 0;
}