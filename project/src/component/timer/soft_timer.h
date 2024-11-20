#ifndef SOFT_TIMER_H
#define SOFT_TIMER_H

#include <signal.h>
#include <time.h>

typedef struct soft_timer_attr
{
    // 公共属性
    timer_t timer_id;
    void (*timer_handler)(union sigval);
    int interval;
    int repeat;
} soft_timer_attr_t;


int create_and_run_timer(soft_timer_attr_t *attr);
int delete_timer(soft_timer_attr_t *attr);
int restart_timer(soft_timer_attr_t *attr);

#endif
