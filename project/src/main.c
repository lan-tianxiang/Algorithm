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

#include "hd_circle_fifo.h"
#include "hd_deque.h"
#include "hd_list.h"
#include "hd_thread.h"
#include "hd_soft_timer.h"

static int count = 0;

void timer_handler(union sigval arg) {
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
    while (count < 10)
    {
    }
    printf("Thread expired %d\n", count);
    delete_timer(&attr);
    printf("Thread ended\n");

    return NULL;
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

    while(1) {
    }

    return 0;
}