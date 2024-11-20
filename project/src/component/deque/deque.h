#ifndef DEQUE_H
#define DEQUE_H

typedef enum {
    DEQUE_ARRAY,
    DEQUE_LIST
} DequeType;

typedef struct Deque Deque;

typedef struct {
    void (*insertFront)(Deque* deque, void* element, int elementSize);
    void (*insertRear)(Deque* deque, void* element, int elementSize);
    void (*deleteFront)(Deque* deque);
    void (*deleteRear)(Deque* deque);
    void* (*getFront)(Deque* deque);
    void* (*getRear)(Deque* deque);
    void (*freeDeque)(Deque* deque);
} DequeOps;

struct Deque {
    void* impl;        // Pointer to the implementation-specific structure
    DequeOps* ops;     // Function pointers to deque operations
};

Deque* createDeque(int elementNum, int maxElementSize, int type);

#endif // DEQUE_H