#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

#define MY_MAX ULLONG_MAX
#define HASH_SIZE 1024

typedef unsigned long long MyInt;

getFunc getFuncPtr;
putFunc putFuncPtr;
Cache* root;
int cacheHitCt;
int cacheAccessCt;

Node *createNode(MyInt num) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->key = num;
    node->value = -1;

    node->prev = NULL;
    node->next = NULL;

    return node;
}

void initializeCache(int capacity, MyInt maxKeys, getFunc chosenGetFunc, putFunc chosenPutFunc) {
    getFuncPtr = chosenGetFunc;
    putFuncPtr = chosenPutFunc;
    
    root = (Cache*)malloc(sizeof(Cache));
    root->cacheCapacity = capacity;
    root->curCacheSize = 0;
    root->head = createNode(0);
    root->tail = createNode(0);

    root->head->next = root->tail;
    root->tail->prev = root->head;

    cacheHitCt = 0;
    cacheAccessCt = 0;
}

void insertNode(Node *node) {
    Node *prevNode = root->tail->prev;
    Node *nxtNode = root->tail;
    prevNode->next = node;
    nxtNode->prev = node;
    node->next = nxtNode;
    node->prev = prevNode;
}

void removeNode(Node *node) {
    Node *prevNode = node->prev;
    Node *nxtNode = node->next;
    prevNode->next = nxtNode;
    nxtNode->prev = prevNode;
}

int naiveLRUGet(MyInt key) {
    cacheAccessCt++;
    
    Node *curNode = root->head->next;
    
    while (curNode != NULL && curNode->key != key) {
        curNode = curNode->next;
    }

    if (curNode == NULL) {
        return -1;
    }

    cacheHitCt++;
    removeNode(curNode);
    insertNode(curNode);
    return curNode->value;
}

void naiveLRUPut(MyInt key, int value) {
    Node *node = createNode(key);
    node->value = value;
    insertNode(node);
    root->curCacheSize += 1;
    
    if (root->curCacheSize > root->cacheCapacity) {
        Node *lruNode = root->head->next;
        // printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, node->key);
        removeNode(lruNode);
        root->curCacheSize -= 1;
        free(lruNode);
    }
}

int naiveFIFOGet(MyInt key) {
    cacheAccessCt++;
    
    Node *curNode = root->head->next;
    
    while (curNode != NULL && curNode->key != key) {
        curNode = curNode->next;
    }

    if (curNode == NULL) {
        return -1;
    }

    cacheHitCt++;

    return curNode->value;
}

void naiveFIFOPut(MyInt key, int value) {
    Node *node = createNode(key);
    node->value = value;
    insertNode(node);
    root->curCacheSize += 1;
    
    if (root->curCacheSize > root->cacheCapacity) {
        Node *lruNode = root->head->next;
        // printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, node->key);
        removeNode(lruNode);
        root->curCacheSize -= 1;
        free(lruNode);
    }
}

void freeCache() {
    Node *current = root->head;
    while (current != NULL) {
        Node *nxt = current->next;
        free(current);
        current = nxt;
    }

    free(root);
    root = NULL;
}

void printCache() {
    Node *current = root->head->next;
    while (current != root->tail) {
        printf("key: %llu, steps: %d\n", current->key, current->value);
        current = current->next;
    }
    printf("\n");
}



// MyInt hash(MyInt key) {
//     return key % HASH_SIZE;
// }

// int lruGet(MyInt key) {
//     int h = hash(key);
//     Node *curNode = root->cacheArr[h];
    
//     if (curNode == NULL) {
//         return -1;
//     } 

//     removeNode(curNode);
//     insertNode(curNode);
//     return curNode->value;
// }

// void lruPut(MyInt key, int value) {
//     int h = hash(key);
//     Node *curNode = root->cacheArr[h];
    
//     if (curNode != NULL) {
//         removeNode(curNode);
//     } 

//     Node *node = createNode(key);
//     node->value = value;
//     root->cacheArr[h] = node;

//     insertNode(node);
//     root->curCacheSize += 1;
    
//     if (root->curCacheSize > root->cacheCapacity) {
//         Node *lruNode = root->head->next;
//         // printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, node->key);
//         removeNode(lruNode);

//         root->cacheArr[lruNode->key] = NULL;

//         free(lruNode);
//     }
// }

// // Chaining Hash Map
// int lruGet(MyInt key) {
//     int h = hash(key);
//     Node *cur = root->cacheArr[h];
    
//     while (cur != NULL && cur->key != key) {
//         cur = cur->next;
//     }

//     if (cur == NULL) {
//         return -1;
//     } 

//     removeNode(cur);
//     insertNode(cur);
//     return cur->value;
// }

// void lruPut(MyInt key, int value) {
//     int h = hash(key);
//     Node *cur = root->cacheArr[h];
//     Node *prev = NULL;

//     while (cur != NULL && cur->key != key) {
//         prev = cur;
//         cur = cur->next;
//     }

//     if (cur != NULL) {
//         cur->value = value;
//         removeNode(cur);
//         insertNode(cur);
//         return;
//     } 

//     Node *newNode = createNode(key);
//     newNode->value = value;

//     if (prev == NULL) {
//         root->cacheArr[h] = newNode;
//     } else {
//         prev->next = newNode;
//     }

//     insertNode(newNode);
//     root->curCacheSize += 1;
    
//     if (root->curCacheSize > root->cacheCapacity) {
//         Node *lruNode = root->head->next;
//         printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, newNode->key);
        
//         removeNode(lruNode);

//         int lruHash = hash(lruNode->key);
//         Node *hashHead = root->cacheArr[lruHash];

//         if (hashHead == lruNode) {
//             root->cacheArr[lruHash] = lruNode->next;
//         } else {
//             Node *prev = NULL;
//             while (hashHead != NULL && hashHead != lruNode) {
//                 prev = hashHead;
//                 hashHead = hashHead->next;
//             }
//             if (prev != NULL) {
//                 prev->next = hashHead->next;
//             }
//         }
//         free(lruNode);
//         root->curCacheSize-= 1;
//     }
// }



// int countSteps(MyInt num) {
//     int count = 0;
//     MyInt curNum = num;
//     // printf("current number: %d, current count: %d\n", curNum, count);

//     while (curNum != 1) {
//         count += 1;
//         curNum = getNextNum(curNum);
//         // printf("current number: %d, current count: %d\n", curNum, count);
//     }
//     // printf("\n");
//     return count;
// }

// int countStepsRecursive(int num) {
//     // base
//     if (num == 1) {
//         return 0;
//     }

//     MyInt nxtNum = getNextNum(num);

//     if (nxtNum == 1) return 1;

//     int steps = 1 + countStepsRecursive(nxtNum);

//     if (putFuncPtr != NULL) {
//         putFuncPtr(num, steps);
//     }

//     return steps;
// }

// int fifoGet(MyInt key) {
//     int h = hash(key);
//     Node *curNode = root->table[h];
    
//     if (curNode != NULL) {
//         return curNode->value;
//     }
//     return -1;
// }

// void fifoPut(MyInt key, int value) {
//     int h = hash(key);
//     Node *curNode = root->table[h];
    
//     if (curNode != NULL) {
//         curNode->value = value;
//         return;
//     }

//     Node *node = createNode(key);
//     node->value = value;
//     insertNode(node);
//     root->curCacheSize += 1;

//     if (root->curCacheSize > root->cacheCapacity) {
//         Node *fifoNode = root->head->next;

//         // printf("Cache memory is full! %d will be replaced by %d\n", fifoNode->key, node->key);

//         removeNode(fifoNode);
//         // cacheMap[fifoNode->key] = NULL;
//         free(fifoNode);
//         root->curCacheSize -= 1;
//     }
// }

// int lruGet(MyInt key) {
//     Node *curNode = cacheMap[key];
    
//     if (curNode != NULL) {
//         // printf("Item %d is already in the Cache. It will become last element\n", key);

//         removeNode(curNode);
//         insertNode(curNode);
//         return curNode->value;
//     }
//     return -1;
// }

// void lruPut(MyInt key, int value) {
//     Node *curNode = cacheMap[key];
    
//     if (curNode != NULL) {
//         curNode->value = value;
//         removeNode(curNode);
//         insertNode(curNode);
//     } else {
//         Node *node = createNode(key);
//         node->value = value;
//         insertNode(node);
//         root->curCacheSize += 1;

//         if (root->curCacheSize > root->cacheCapacity) {
//             Node *lruNode = root->head->next;

//             // printf("Cache memory is full! %d will be replaced by %d\n", lruNode->key, node->key);

//             removeNode(lruNode);
//             cacheMap[lruNode->key] = NULL;
//             free(lruNode);
//             root->curCacheSize -= 1;
//         }
//     }
// }

// int fifoGet(MyInt key) {
//     Node *curNode = cacheMap[key];
    
//     if (curNode != NULL) {
//         return curNode->value;
//     }
//     return -1;
// }

// void fifoPut(MyInt key, int value) {
//     Node *curNode = cacheMap[key];
    
//     if (curNode != NULL) {
//         curNode->value = value;
//         return;
//     }

//     Node *node = createNode(key);
//     node->value = value;
//     insertNode(node);
//     root->curCacheSize += 1;

//     if (root->curCacheSize > root->cacheCapacity) {
//         Node *fifoNode = root->head->next;

//         // printf("Cache memory is full! %d will be replaced by %d\n", fifoNode->key, node->key);

//         removeNode(fifoNode);
//         cacheMap[fifoNode->key] = NULL;
//         free(fifoNode);
//         root->curCacheSize -= 1;
//     }
// }

// int lruGet(MyInt key) {
//     Node *curNode = root->head;

//     while (curNode != NULL && curNode->key != key) {
//         curNode = curNode->next;
//     }
    
//     if (curNode != NULL) {
//         // printf("Item %d is already in the Cache. It will become last element\n", key);

//         removeNode(curNode);
//         insertNode(curNode);
//         return curNode->value;
//     }
//     return -1;
// }

// void lruPut(MyInt key, int value) {
//     Node *curNode = root->head;

//     while (curNode != NULL && curNode->key != key) {
//         curNode = curNode->next;
//     }
    
//     if (curNode != NULL) {
//         curNode->value = value;
//         removeNode(curNode);
//         insertNode(curNode);
//     } else {
//         Node *node = createNode(key);
//         node->value = value;
//         insertNode(node);
//         root->curCacheSize += 1;

//         if (root->curCacheSize > root->cacheCapacity) {
//             Node *lruNode = root->head->next;

//             // printf("Cache memory is full! %d will be replaced by %d\n", lruNode->key, node->key);

//             removeNode(lruNode);
//             cacheMap[lruNode->key] = NULL;
//             free(lruNode);
//             root->curCacheSize -= 1;
//         }
//     }
// }

// int fifoGet(MyInt key) {
//     Node *curNode = root->head;

//     while (curNode != NULL && curNode->key != key) {
//         curNode = curNode->next;
//     }
    
//     if (curNode != NULL) {
//         return curNode->value;
//     }
//     return -1;
// }

// void fifoPut(MyInt key, int value) {
//     Node *curNode = root->head;

//     while (curNode != NULL && curNode->key != key) {
//         curNode = curNode->next;
//     }
    
//     if (curNode != NULL) {
//         curNode->value = value;
//         return;
//     }

//     Node *node = createNode(key);
//     node->value = value;
//     insertNode(node);
//     root->curCacheSize += 1;

//     if (root->curCacheSize > root->cacheCapacity) {
//         Node *fifoNode = root->head->next;

//         // printf("Cache memory is full! %d will be replaced by %d\n", fifoNode->key, node->key);

//         removeNode(fifoNode);
//         cacheMap[fifoNode->key] = NULL;
//         free(fifoNode);
//         root->curCacheSize -= 1;
//     }
// }

// int countStepsRecursive(MyInt num) {
//     // base
//     if (num == 1) {
//         return 0;
//     } 
    
//     int cachedValue = -1;
    
//     if (getFuncPtr != NULL) {
//         cachedValue = getFuncPtr(num);
//     }

//     if (cachedValue != -1) {
//         // printf("cache hit! num: %d\n", num);
//         cacheHitCt += 1;
//         return cachedValue;
//     }

//     MyInt nxtNum = getNextNum(num);

//     if (nxtNum == 1) return 1;

//     int steps = 1 + countStepsRecursive(nxtNum);

//     if (putFuncPtr != NULL) {
//         putFuncPtr(num, steps);
//     }

//     return steps;
// }

// int countStepsIterative(MyInt num) {
//     // base
//     if (num == 1) {
//         return 0;
//     }

//     int steps = 0;

//     while (num != 1) {
//         MyInt nxtNum = getNextNum(num);
//         if (nxtNum == 1) return 1;

//     }
    
//     int cachedValue = -1;
    
//     if (getFuncPtr != NULL) {
//         cachedValue = getFuncPtr(num);
//     }

//     if (cachedValue != -1) {
//         // printf("cache hit! num: %d\n", num);
//         cacheHitCt += 1;
//         return cachedValue;
//     }

//     MyInt nxtNum = getNextNum(num);

//     if (nxtNum == 1) return 1;

//     int steps = 1 + countStepsRecursive(nxtNum);

//     if (putFuncPtr != NULL) {
//         putFuncPtr(num, steps);
//     }

//     return steps;
// }