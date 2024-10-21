#ifndef DEQUE_ARRAY_H
#define DEQUE_ARRAY_H

typedef struct {
    void* arr;         // 指向存储队列元素的数组
    int front;         // 指向队列前端的索引
    int rear;          // 指向队列后端的索引
    int elementNum;    // 队列中元素的数量
    int elementSize;   // 每个元素的大小
} Deque;

// 创建一个双端队列
Deque* createDeque(int elementNum, int elementSize);

// 在队列前端插入元素
void insertFront(Deque* deque, void* element);

// 在队列后端插入元素
void insertRear(Deque* deque, void* element);

// 从队列前端删除元素
void deleteFront(Deque* deque);

// 从队列后端删除元素
void deleteRear(Deque* deque);

// 获取队列前端的元素
void* getFront(Deque* deque);

// 获取队列后端的元素
void* getRear(Deque* deque);

// 释放双端队列
void freeDeque(Deque* deque);

void printElement(void* element, int elementSize);

#endif // DEQUE_ARRAY_H