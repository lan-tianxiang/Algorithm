/*
    FILE NAME: deque_array.c
    PURPOSE: Implementation of Deque using Array
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include "deque_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implement createDequeArray function
DequeArray* createDequeArray(int elementNum, int maxElementSize) {
    DequeArray* dequeArray = (DequeArray*)malloc(sizeof(DequeArray));
    if (!dequeArray) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    dequeArray->arr = malloc(elementNum * maxElementSize);
    if (!dequeArray->arr) {
        printf("Memory allocation failed\n");
        free(dequeArray);
        return NULL;
    }
    dequeArray->front = -1;
    dequeArray->rear = 0;
    dequeArray->elementNum = elementNum;
    dequeArray->maxElementSize = maxElementSize;
    return dequeArray;
}

// Update isFull function
static inline int isFull(DequeArray* deque) {
    return ((deque->front == 0 && deque->rear == deque->elementNum - 1) ||
            (deque->front == deque->rear + 1));
}

// Update isEmpty function
static inline int isEmpty(DequeArray* deque) {
    return (deque->front == -1);
}

// Implement insertFrontArray function
void insertFrontArray(Deque* deque, void* element, int elementSize) {
    DequeArray* d = (DequeArray*)deque->impl;
    if (isFull(d)) {
        printf("Overflow\n");
        return;
    }

    if (d->front == -1) {
        d->front = 0;
        d->rear = 0;
    } else if (d->front == 0) {
        d->front = d->elementNum - 1;
    } else {
        d->front = d->front - 1;
    }

    memcpy((char*)d->arr + d->front * d->maxElementSize, element, elementSize);
}

// Implement insertRearArray function
void insertRearArray(Deque* deque, void* element, int elementSize) {
    DequeArray* d = (DequeArray*)deque->impl;
    if (isFull(d)) {
        printf("Overflow\n");
        return;
    }

    if (d->front == -1) {
        d->front = 0;
        d->rear = 0;
    } else if (d->rear == d->elementNum - 1) {
        d->rear = 0;
    } else {
        d->rear = d->rear + 1;
    }

    memcpy((char*)d->arr + d->rear * d->maxElementSize, element, elementSize);
}

// Implement deleteFrontArray function
void deleteFrontArray(Deque* deque) {
    DequeArray* d = (DequeArray*)deque->impl;
    if (isEmpty(d)) {
        printf("Queue Underflow\n");
        return;
    }

    if (d->front == d->rear) {
        d->front = -1;
        d->rear = -1;
    } else if (d->front == d->elementNum - 1) {
        d->front = 0;
    } else {
        d->front = d->front + 1;
    }
}

// Implement deleteRearArray function
void deleteRearArray(Deque* deque) {
    DequeArray* d = (DequeArray*)deque->impl;
    if (isEmpty(d)) {
        printf("Queue Underflow\n");
        return;
    }

    if (d->front == d->rear) {
        d->front = -1;
        d->rear = -1;
    } else if (d->rear == 0) {
        d->rear = d->elementNum - 1;
    } else {
        d->rear = d->rear - 1;
    }
}

// Implement getFrontArray function
void* getFrontArray(Deque* deque) {
    DequeArray* d = (DequeArray*)deque->impl;
    if (isEmpty(d)) {
        printf("Queue Underflow\n");
        return NULL;
    }
    return (char*)d->arr + d->front * d->maxElementSize;
}

// Implement getRearArray function
void* getRearArray(Deque* deque) {
    DequeArray* d = (DequeArray*)deque->impl;
    if (isEmpty(d) || d->rear < 0) {
        printf("Queue Underflow\n");
        return NULL;
    }
    return (char*)d->arr + d->rear * d->maxElementSize;
}

// Implement freeDequeArray function
void freeDequeArray(Deque* deque) {
    DequeArray* d = (DequeArray*)deque->impl;
    if (d->arr) {
        free(d->arr);
    }
    free(d);
    free(deque->ops);
    free(deque);
}
