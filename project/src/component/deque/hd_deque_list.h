#ifndef HD_DEQUE_LIST_H
#define HD_DEQUE_LIST_H

#include "hd_deque.h"

typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct {
    Node* front;        // Pointer to the front node
    Node* rear;         // Pointer to the rear node
    int maxElementSize; // Size of each element
} DequeList;

// Create a new deque
DequeList* createDequeList(int maxElementSize);

// Insert an element at the front
void insertFrontList(Deque* deque, void* element, int elementSize);

// Insert an element at the rear
void insertRearList(Deque* deque, void* element, int elementSize);

// Delete an element from the front
void deleteFrontList(Deque* deque);

// Delete an element from the rear
void deleteRearList(Deque* deque);

// Get the front element
void* getFrontList(Deque* deque);

// Get the rear element
void* getRearList(Deque* deque);

// Free the deque
void freeDequeList(Deque* deque);

#endif // HD_DEQUE_LIST_H