#ifndef DEQUE_ARRAY_H
#define DEQUE_ARRAY_H

#include "deque.h"

typedef struct {
    void *arr;          // 指向存储队列元素的数组
    int front;          // 指向队列前端的索引
    int rear;           // 指向队列后端的索引
    int elementNum;     // 队列中元素的数量
    int maxElementSize; // 每个元素的大小
} DequeArray;

// 创建一个双端队列
DequeArray* createDequeArray(int elementNum, int maxElementSize);

// 在队列前端插入元素
void insertFrontArray(Deque* deque, void* element, int elementSize);

// 在队列后端插入元素
void insertRearArray(Deque* deque, void* element, int elementSize);

// 从队列前端删除元素
void deleteFrontArray(Deque* deque);

// 从队列后端删除元素
void deleteRearArray(Deque* deque);

// 获取队列前端的元素
void* getFrontArray(Deque* deque);

// 获取队列后端的元素
void* getRearArray(Deque* deque);

// 释放双端队列
void freeDequeArray(Deque* deque);

#endif // DEQUE_ARRAY_H