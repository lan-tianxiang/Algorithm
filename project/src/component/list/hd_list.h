#ifndef HD_LIST_H
#define HD_LIST_H

typedef struct Node {
    int dataSize;    // Add dataSize to store size of data
    struct Node* next;
    char data[];  // 变长数组，必须是结构体的最后一个成员
} Node;

typedef struct LinkedList {
    Node* head;
    int length;      // Add length to track the number of nodes
} LinkedList;

// 初始化链表
void initList(LinkedList* list);

// 插入节点
void insertAtHead(LinkedList* list, void* data, int dataSize);
void insertAtTail(LinkedList* list, void* data, int dataSize);
void insertAtPosition(LinkedList* list, void* data, int dataSize, int position);

// 删除节点
void deleteAtHead(LinkedList* list);
void deleteAtTail(LinkedList* list);
void deleteAtPosition(LinkedList* list, int position);
void deleteByValue(LinkedList* list, void* value, int (*cmp)(void*, void*));

// 查找节点
Node* findByValue(LinkedList* list, void* value, int (*cmp)(void*, void*));
Node* findByPosition(LinkedList* list, int position);

// 更新节点
void updateNode(LinkedList* list, int position, void* newValue, int dataSize);

// 遍历链表
void traverseList(LinkedList* list, void (*printFunc)(void*));

// 链表反转
void reverseList(LinkedList* list);

// 获取链表长度
int getListLength(LinkedList* list);

// 判空操作
int isEmpty(LinkedList* list);

// 合并链表
void mergeLists(LinkedList* list1, LinkedList* list2);

// 拆分链表
void splitList(LinkedList* list, int position, LinkedList* newList);

// 复制链表
LinkedList* copyList(LinkedList* list);

// 排序链表
void sortList(LinkedList* list, int (*cmp)(void*, void*));

// 释放链表
void freeList(LinkedList* list);

#endif // HD_LIST_H
