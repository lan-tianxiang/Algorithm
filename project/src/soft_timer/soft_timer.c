#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define TIMER_HANDLER_TYPE 1 // 0: signal handler, 1: thread handler

#if TIMER_HANDLER_TYPE == 0
void timer_handler(int sig, siginfo_t *siginfo, void *data){
    printf("Timer expired!\n");
}
#elif TIMER_HANDLER_TYPE == 1
void timer_handler(union sigval val) {
    printf("Timer expired!\n");
}
#endif

void* timer_thread(void* arg) {
    timer_t timer_id;
    int ret;

#if TIMER_HANDLER_TYPE == 0
    // 设置信号处理函数
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    ret = sigaction(SIGRTMIN, &sa, NULL);
    if (ret != 0) {
        perror("sigaction");
        return NULL;
    }

    // 创建定时器
    ret = timer_create(CLOCK_REALTIME, NULL, &timer_id);
    if (ret != 0) {
        perror("timer_create");
        return NULL;
    }
#elif TIMER_HANDLER_TYPE == 1
    // 设置定时器事件
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timer_id;
    sev.sigev_notify_function = timer_handler;
    sev.sigev_notify_attributes = NULL; // 使用默认线程属性

    // 创建定时器
    ret = timer_create(CLOCK_REALTIME, &sev, &timer_id);
    if (ret != 0) {
        perror("timer_create");
        return NULL;
    }
#endif

    // 设置定时器时间
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 1; // 5秒后触发
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0; // 不重复
    timer_spec.it_interval.tv_nsec = 0;
    ret = timer_settime(timer_id, 0, &timer_spec, NULL);
    if (ret != 0) {
        perror("timer_settime");
        return NULL;
    }
#if TIMER_HANDLER_TYPE == 0
    // 等待定时器触发
    while (1) {
        pause();
    }
#elif TIMER_HANDLER_TYPE == 1
    // 等待定时器触发
    sleep(timer_spec.it_value.tv_sec + 1);
#endif
    return NULL;
}

int main() {
    pthread_t thread_id;

    // 创建线程
    if (pthread_create(&thread_id, NULL, timer_thread, NULL) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    // 等待线程结束
    pthread_join(thread_id, NULL);

    return 0;
}