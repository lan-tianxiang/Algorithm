#ifndef DEQUE_ARRAY_H
#define DEQUE_ARRAY_H

typedef struct {
    int *arr;
    int front;
    int rear;
    int size;
} Deque;

// 创建一个双端队列
Deque* createDeque(int size);

// 在队列前端插入元素
void insertFront(Deque* deque, int key);

// 在队列后端插入元素
void insertRear(Deque* deque, int key);

// 从队列前端删除元素
void deleteFront(Deque* deque);

// 从队列后端删除元素
void deleteRear(Deque* deque);

// 获取队列前端的元素
int getFront(Deque* deque);

// 获取队列后端的元素
int getRear(Deque* deque);

// 释放双端队列
void freeDeque(Deque* deque);

#endif // DEQUE_ARRAY_H