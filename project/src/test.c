/*
    FILE NAME: test.c
    PURPOSE: Test file for Deque using Array
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include <stdio.h>
#include <stdlib.h>
#include "deque.h" // 假设双端队列的实现放在 deque.h 和 deque.c 中

void testInsertFront() {
    Deque* deque = createDeque(5);
    insertFront(deque, 10);
    insertFront(deque, 20);
    insertFront(deque, 30);
    printf("Front element after insertFront operations: %d\n", getFront(deque)); // 应该输出 30
    freeDeque(deque);
}

void testInsertRear() {
    Deque* deque = createDeque(5);
    insertRear(deque, 10);
    insertRear(deque, 20);
    insertRear(deque, 30);
    printf("Rear element after insertRear operations: %d should be 30\n", getRear(deque)); // 应该输出 30
    freeDeque(deque);
}

void testDeleteFront() {
    Deque* deque = createDeque(5);
    insertRear(deque, 10);
    insertRear(deque, 20);
    insertRear(deque, 30);
    deleteFront(deque);
    printf("Front element after deleteFront operation: %d should be 20\n", getFront(deque)); // 应该输出 20
    freeDeque(deque);
}

void testDeleteRear() {
    Deque* deque = createDeque(5);
    insertRear(deque, 10);
    insertRear(deque, 20);
    insertRear(deque, 30);
    deleteRear(deque);
    printf("Rear element after deleteRear operation: %d should be 20\n", getRear(deque)); // 应该输出 20
    freeDeque(deque);
}

void testGetFront() {
    Deque* deque = createDeque(5);
    insertRear(deque, 10);
    insertRear(deque, 20);
    printf("Front element: %d should be 10\n", getFront(deque)); // 应该输出 10
    freeDeque(deque);
}

void testGetRear() {
    Deque* deque = createDeque(5);
    insertRear(deque, 10);
    insertRear(deque, 20);
    printf("Rear element: %d should be 20\n", getRear(deque)); // 应该输出 20
    freeDeque(deque);
}

int main() {
    testInsertFront();
    testInsertRear();
    testDeleteFront();
    testDeleteRear();
    testGetFront();
    testGetRear();
#ifdef HELLO_SHARED_IMPLEMENTATION
    printf("Hello from shared implementation!\n");
#endif
    return 0;
}