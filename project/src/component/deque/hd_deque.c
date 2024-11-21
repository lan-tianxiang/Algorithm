/*
    FILE NAME: hd_deque.c
    PURPOSE: Implementation of Deque
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include "hd_deque.h"
#include "hd_deque_array.h"
#include "hd_deque_list.h"
#include <stdlib.h>

Deque* createDeque(int elementNum, int maxElementSize, int type) {
    Deque* deque = (Deque*)malloc(sizeof(Deque));
    if (!deque) {
        // Handle memory allocation failure
        return NULL;
    }
    deque->ops = (DequeOps*)malloc(sizeof(DequeOps));
    if (!deque->ops) {
        free(deque);
        return NULL;
    }

    if (type == DEQUE_ARRAY) {
        deque->impl = createDequeArray(elementNum, maxElementSize);
        deque->ops->insertFront = insertFrontArray;
        deque->ops->insertRear = insertRearArray;
        deque->ops->deleteFront = deleteFrontArray;
        deque->ops->deleteRear = deleteRearArray;
        deque->ops->getFront = getFrontArray;
        deque->ops->getRear = getRearArray;
        deque->ops->freeDeque = freeDequeArray;
    } else if (type == DEQUE_LIST) {
        deque->impl = createDequeList(maxElementSize);
        deque->ops->insertFront = insertFrontList;
        deque->ops->insertRear = insertRearList;
        deque->ops->deleteFront = deleteFrontList;
        deque->ops->deleteRear = deleteRearList;
        deque->ops->getFront = getFrontList;
        deque->ops->getRear = getRearList;
        deque->ops->freeDeque = freeDequeList;
    } else {
        free(deque->ops);
        free(deque);
        return NULL;
    }

    return deque;
}