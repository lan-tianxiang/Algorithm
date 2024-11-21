#include "hd_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // 添加头文件以使用 memcpy

// 初始化链表
void initList(LinkedList* list) {
    list->head = NULL;
    list->length = 0;    // Initialize length
}

// 插入节点
void insertAtHead(LinkedList* list, void* data, int dataSize) {
    Node* newNode = (Node*)malloc(sizeof(Node) + dataSize);  // 分配足够的内存，包括节点和数据
    newNode->dataSize = dataSize;                            // 存储数据大小
    newNode->next = list->head;                              // 将新节点的 next 指向当前头节点
    memcpy(newNode->data, data, dataSize);                   // 复制数据到变长数组
    list->head = newNode;                                    // 更新头节点为新节点
    list->length++;                                          // 增加链表长度
}

void insertAtTail(LinkedList* list, void* data, int dataSize) {
    Node* newNode = (Node*)malloc(sizeof(Node) + dataSize);
    newNode->dataSize = dataSize;   // Store dataSize
    newNode->next = NULL;
    memcpy(newNode->data, data, dataSize);
    if (list->head == NULL) {
        list->head = newNode;
    } else {
        Node* temp = list->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    list->length++;                 // Increment length
}

void insertAtPosition(LinkedList* list, void* data, int dataSize, int position) {
    if (position == 0) {
        insertAtHead(list, data, dataSize);
        return;
    }
    Node* temp = list->head;
    for (int i = 0; i < position - 1 && temp != NULL; i++) {
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("Position out of bounds\n");
        return;
    }
    Node* newNode = (Node*)malloc(sizeof(Node) + dataSize);
    newNode->dataSize = dataSize;   // Store dataSize
    newNode->next = temp->next;
    memcpy(newNode->data, data, dataSize);
    temp->next = newNode;
    list->length++;                 // Increment length
}

// 删除节点
void deleteAtHead(LinkedList* list) {
    if (list->head == NULL) return;
    Node* temp = list->head;
    list->head = list->head->next;
    free(temp);
    list->length--;                 // Decrement length
}

void deleteAtTail(LinkedList* list) {
    if (list->head == NULL) return;
    if (list->head->next == NULL) {
        free(list->head);
        list->head = NULL;
        list->length--;             // Decrement length
        return;
    }
    Node* temp = list->head;
    while (temp->next->next != NULL) {
        temp = temp->next;
    }
    free(temp->next);
    temp->next = NULL;
    list->length--;                 // Decrement length
}

void deleteAtPosition(LinkedList* list, int position) {
    if (position == 0) {
        deleteAtHead(list);
        return;
    }
    Node* temp = list->head;
    for (int i = 0; i < position - 1 && temp != NULL; i++) {
        temp = temp->next;
    }
    if (temp == NULL || temp->next == NULL) {
        printf("Position out of bounds\n");
        return;
    }
    Node* nodeToDelete = temp->next;
    temp->next = temp->next->next;
    free(nodeToDelete);
    list->length--;                 // Decrement length
}

void deleteByValue(LinkedList* list, void* value, int (*cmp)(void*, void*)) {
    if (list->head == NULL) return;
    Node* temp = list->head;
    Node* prev = NULL;
    while (temp != NULL) {
        if (cmp(temp->data, value) == 0) {
            if (prev == NULL) {
                list->head = temp->next;
            } else {
                prev->next = temp->next;
            }
            free(temp);
            list->length--;         // Decrement length
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Value not found\n");
}

// 查找节点
Node* findByValue(LinkedList* list, void* value, int (*cmp)(void*, void*)) {
    Node* temp = list->head;
    while (temp != NULL) {
        if (cmp(temp->data, value) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

Node* findByPosition(LinkedList* list, int position) {
    Node* temp = list->head;
    for (int i = 0; i < position && temp != NULL; i++) {
        temp = temp->next;
    }
    return temp;
}

// 更新节点
void updateNode(LinkedList* list, int position, void* newValue, int dataSize) {
    Node* temp = findByPosition(list, position);
    if (temp != NULL) {
        if (dataSize == temp->dataSize) {
            memcpy(temp->data, newValue, dataSize);
        } else {
            deleteAtPosition(list, position);
            insertAtPosition(list, newValue, dataSize, position);
        }
    } else {
        printf("Position out of bounds\n");
    }
}

// 遍历链表
void traverseList(LinkedList* list, void (*printFunc)(void*)) {
    Node* temp = list->head;
    while (temp != NULL) {
        printFunc(temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

// 链表反转
void reverseList(LinkedList* list) {
    Node* prev = NULL;
    Node* current = list->head;
    Node* next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    list->head = prev;
}

// 获取链表长度
int getListLength(LinkedList* list) {
    return list->length;
}

// 判空操作
int isEmpty(LinkedList* list) {
    return list->head == NULL;
}

// 合并链表
void mergeLists(LinkedList* list1, LinkedList* list2) {
    if (list1->head == NULL) {
        list1->head = list2->head;
        return;
    }
    Node* temp = list1->head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = list2->head;
}

// 拆分链表
void splitList(LinkedList* list, int position, LinkedList* newList) {
    if (position == 0) {
        newList->head = list->head;
        list->head = NULL;
        return;
    }
    Node* temp = list->head;
    for (int i = 0; i < position - 1 && temp != NULL; i++) {
        temp = temp->next;
    }
    if (temp == NULL || temp->next == NULL) {
        printf("Position out of bounds\n");
        return;
    }
    newList->head = temp->next;
    temp->next = NULL;
}

// 复制链表
LinkedList* copyList(LinkedList* list) {
    LinkedList* newList = (LinkedList*)malloc(sizeof(LinkedList));
    initList(newList);
    Node* temp = list->head;
    while (temp != NULL) {
        insertAtTail(newList, temp->data, temp->dataSize);
        temp = temp->next;
    }
    return newList;
}

// Add function to free the entire list
void freeList(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* nextNode = current->next;
        free(current);
        current = nextNode;
    }
    list->head = NULL;
    list->length = 0;   // Reset length
}
