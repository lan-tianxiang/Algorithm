/*
    FILE NAME: deque_array.c
    PURPOSE: Implementation of Deque using Array
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include "deque_array.h"
#include <stdio.h>
#include <stdlib.h>

// 创建一个双端队列
Deque* createDeque(int size) {
    Deque* deque = (Deque*)malloc(sizeof(Deque));
    if (!deque) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    deque->arr = (int*)malloc(size * sizeof(int));
    if (!deque->arr) {
        printf("Memory allocation failed\n");
        free(deque);
        return NULL;
    }
    deque->front = -1;
    deque->rear = 0;
    deque->size = size;
    return deque;
}

// 检查双端队列是否已满
static int isFull(Deque* deque) {
    return ((deque->front == 0 && deque->rear == deque->size - 1) || 
            deque->front == deque->rear + 1);
}

// 检查双端队列是否为空
static int isEmpty(Deque* deque) {
    return (deque->front == -1);
}

// 在队列前端插入元素
void insertFront(Deque* deque, int key) {
    if (isFull(deque)) {
        printf("Overflow\n");
        return;
    }

    if (deque->front == -1) {
        deque->front = 0;
        deque->rear = 0;
    } else if (deque->front == 0) {
        deque->front = deque->size - 1;
    } else {
        deque->front = deque->front - 1;
    }

    deque->arr[deque->front] = key;
}

// 在队列后端插入元素
void insertRear(Deque* deque, int key) {
    if (isFull(deque)) {
        printf("Overflow\n");
        return;
    }

    if (deque->front == -1) {
        deque->front = 0;
        deque->rear = 0;
    } else if (deque->rear == deque->size - 1) {
        deque->rear = 0;
    } else {
        deque->rear = deque->rear + 1;
    }

    deque->arr[deque->rear] = key;
}

// 从队列前端删除元素
void deleteFront(Deque* deque) {
    if (isEmpty(deque)) {
        printf("Queue Underflow\n");
        return;
    }

    if (deque->front == deque->rear) {
        deque->front = -1;
        deque->rear = -1;
    } else if (deque->front == deque->size - 1) {
        deque->front = 0;
    } else {
        deque->front = deque->front + 1;
    }
}

// 从队列后端删除元素
void deleteRear(Deque* deque) {
    if (isEmpty(deque)) {
        printf("Queue Underflow\n");
        return;
    }

    if (deque->front == deque->rear) {
        deque->front = -1;
        deque->rear = -1;
    } else if (deque->rear == 0) {
        deque->rear = deque->size - 1;
    } else {
        deque->rear = deque->rear - 1;
    }
}

// 获取队列前端的元素
int getFront(Deque* deque) {
    if (isEmpty(deque)) {
        printf("Queue Underflow\n");
        return -1;
    }
    return deque->arr[deque->front];
}

// 获取队列后端的元素
int getRear(Deque* deque) {
    if (isEmpty(deque) || deque->rear < 0) {
        printf("Queue Underflow\n");
        return -1;
    }
    return deque->arr[deque->rear];
}

// 释放双端队列
void freeDeque(Deque* deque) {
    if (deque) {
        if (deque->arr) {
            free(deque->arr);
        }
        free(deque);
    }
}