/*
    FILE NAME: hd_thread.c
    PURPOSE: Implementation of thread
    DATE: 2024/08/14
    AUTHOR: lan-tianxiang
*/

#include "hd_thread.h"

#include <stdio.h>
#include <stdlib.h>

int create_and_run_thread(thread_attr_t *attr)
{
    pthread_attr_t thread_attr;

    // 初始化线程属性
    if (pthread_attr_init(&thread_attr) != 0)
    {
        perror("pthread_attr_init");
        return EXIT_FAILURE;
    }

    // 设置线程栈大小
    if (pthread_attr_setstacksize(&thread_attr, attr->stack_size) != 0)
    {
        perror("pthread_attr_setstacksize");
        pthread_attr_destroy(&thread_attr);
        return EXIT_FAILURE;
    }

    // 设置线程调度策略
    if (pthread_attr_setschedpolicy(&thread_attr, attr->schedpolicy) != 0)
    {
        perror("pthread_attr_setschedpolicy");
        pthread_attr_destroy(&thread_attr);
        return EXIT_FAILURE;
    }

    // 设置线程优先级
    struct sched_param param;
    param.sched_priority = attr->priority; // 设置优先级
    if (pthread_attr_setschedparam(&thread_attr, &param) != 0)
    {
        perror("pthread_attr_setschedparam");
        pthread_attr_destroy(&thread_attr);
        return EXIT_FAILURE;
    }

    // 创建线程并传递属性
    if (pthread_create(&attr->thread, &thread_attr, attr->thread_func, attr->arg) != 0)
    {
        perror("pthread_create");
        pthread_attr_destroy(&thread_attr);
        return EXIT_FAILURE;
    }

    // 销毁线程属性
    if (pthread_attr_destroy(&thread_attr) != 0)
    {
        perror("pthread_attr_destroy");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
