/*
    FILE NAME: hd_deque_list.c
    PURPOSE: Implementation of Deque using linked list
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include "hd_deque_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implement createDequeList function
DequeList* createDequeList(int maxElementSize) {
    DequeList* dequeList = (DequeList*)malloc(sizeof(DequeList));
    if (!dequeList) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    dequeList->front = NULL;
    dequeList->rear = NULL;
    dequeList->maxElementSize = maxElementSize;
    return dequeList;
}

// Implement insertFrontList function
void insertFrontList(Deque* deque, void* element, int elementSize) {
    DequeList* d = (DequeList*)deque->impl;
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->data = malloc(elementSize);
    if (!newNode->data) {
        printf("Memory allocation failed\n");
        free(newNode);
        return;
    }
    memcpy(newNode->data, element, elementSize);
    newNode->prev = NULL;
    newNode->next = d->front;

    if (d->front) {
        d->front->prev = newNode;
    } else {
        d->rear = newNode;
    }
    d->front = newNode;
}

// Implement insertRearList function
void insertRearList(Deque* deque, void* element, int elementSize) {
    DequeList* d = (DequeList*)deque->impl;
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->data = malloc(elementSize);
    if (!newNode->data) {
        printf("Memory allocation failed\n");
        free(newNode);
        return;
    }
    memcpy(newNode->data, element, elementSize);
    newNode->next = NULL;
    newNode->prev = d->rear;

    if (d->rear) {
        d->rear->next = newNode;
    } else {
        d->front = newNode;
    }
    d->rear = newNode;
}

// Implement deleteFrontList function
void deleteFrontList(Deque* deque) {
    DequeList* d = (DequeList*)deque->impl;
    if (!d->front) {
        printf("Queue Underflow\n");
        return;
    }
    Node* temp = d->front;
    d->front = d->front->next;
    if (d->front) {
        d->front->prev = NULL;
    } else {
        d->rear = NULL;
    }
    free(temp->data);
    free(temp);
}

// Implement deleteRearList function
void deleteRearList(Deque* deque) {
    DequeList* d = (DequeList*)deque->impl;
    if (!d->rear) {
        printf("Queue Underflow\n");
        return;
    }
    Node* temp = d->rear;
    d->rear = d->rear->prev;
    if (d->rear) {
        d->rear->next = NULL;
    } else {
        d->front = NULL;
    }
    free(temp->data);
    free(temp);
}

// Implement getFrontList function
void* getFrontList(Deque* deque) {
    DequeList* d = (DequeList*)deque->impl;
    if (!d->front) {
        printf("Queue Underflow\n");
        return NULL;
    }
    return d->front->data;
}

// Implement getRearList function
void* getRearList(Deque* deque) {
    DequeList* d = (DequeList*)deque->impl;
    if (!d->rear) {
        printf("Queue Underflow\n");
        return NULL;
    }
    return d->rear->data;
}

// Implement freeDequeList function
void freeDequeList(Deque* deque) {
    DequeList* d = (DequeList*)deque->impl;
    Node* current = d->front;
    while (current) {
        Node* next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    free(d);
    free(deque->ops);
    free(deque);
}
