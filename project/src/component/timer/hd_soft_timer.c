#include "hd_soft_timer.h"
#include <stdio.h>
#include <stdlib.h>

int create_and_run_timer(soft_timer_attr_t *attr)
{
    int ret = 0;

    // 设置定时器事件
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &attr->timer_id;
    sev.sigev_notify_function = attr->timer_handler;
    sev.sigev_notify_attributes = NULL; // 使用默认线程属性

    // 创建定时器
    ret = timer_create(CLOCK_REALTIME, &sev, &attr->timer_id);
    if (ret != 0) {
        perror("timer_create");
        return ret;
    }

    // 设置定时器时间
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = attr->interval;
    timer_spec.it_value.tv_nsec = 0;

    if (attr->repeat)
    {
        timer_spec.it_interval.tv_sec = attr->interval;
        timer_spec.it_interval.tv_nsec = 0;
    }
    else
    {
        timer_spec.it_interval.tv_sec = 0; // 不重复
        timer_spec.it_interval.tv_nsec = 0;
    }

    ret = timer_settime(attr->timer_id, 0, &timer_spec, NULL);
    if (ret != 0) {
        perror("timer_settime");
        return ret;
    }

    return ret;
}

int delete_timer(soft_timer_attr_t *attr)
{
    int ret = 0;

    ret = timer_delete(attr->timer_id);
    if (ret != 0) {
        perror("timer_delete");
        return ret;
    }

    return ret;
}

int restart_timer(soft_timer_attr_t *attr)
{
    int ret = 0;

    ret = delete_timer(attr);
    if (ret != 0) {
        perror("delete_timer");
        return ret;
    }

    // 设置定时器时间
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = attr->interval;
    timer_spec.it_value.tv_nsec = 0;

    if (attr->repeat)
    {
        timer_spec.it_interval.tv_sec = attr->interval;
        timer_spec.it_interval.tv_nsec = 0;
    }
    else
    {
        timer_spec.it_interval.tv_sec = 0; // 不重复
        timer_spec.it_interval.tv_nsec = 0;
    }

    ret = timer_settime(attr->timer_id, 0, &timer_spec, NULL);
    if (ret != 0) {
        perror("timer_settime");
        return ret;
    }

    return ret;
}
