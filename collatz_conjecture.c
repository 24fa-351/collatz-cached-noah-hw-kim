#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MY_MAX ULLONG_MAX
#define HASH_SIZE 1024

typedef unsigned long long MyInt; 
typedef int (*getFunc)(MyInt);
typedef void (*putFunc)(MyInt, int);

typedef struct node
{
    MyInt key;
    int value;
    struct node* next;
    struct node* prev;
} Node;

typedef struct llist
{
    Node* head;
    Node* tail;
    int listLength;
} Llist;

typedef struct cache
{
    int cacheCapacity;
    int curCacheSize;
    Llist* linkedList;
} Cache;

getFunc getFuncPtr;
putFunc putFuncPtr;
Cache *root;
int cacheHitCt;
Llist** table;

Node *createNode(MyInt num);
void initializeCache(int capacity, MyInt maxKeys, getFunc chosenGetFunc, putFunc chosenPutFunc);
void insertNode(Llist *llist,Node *node);
void removeNode(Node *node);
int lruGet(MyInt key);
void lruPut(MyInt key, int value);
int fifoGet(MyInt key);
void fifoPut(MyInt key, int value);

MyInt getRandomNum(MyInt minNum, MyInt maxNum);
MyInt getNextNum(MyInt num);
int countStepsRecursive(MyInt num);
int countStepsWrapper(MyInt num);
void freeCache();
void printCache();

Node *createNode(MyInt num) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->key = num;
    node->value = -1;

    node->prev = NULL;
    node->next = NULL;

    return node;
}

Llist *createLlist() {
    Llist *llist = (Llist*)malloc(sizeof(Llist));
    llist->head = createNode(0);
    llist->tail = createNode(0);
    llist->listLength = 0;

    llist->head->next = llist->tail;
    llist->tail->prev = llist->head;

    return llist;
}

void initializeCache(int capacity, MyInt maxKeys, getFunc chosenGetFunc, putFunc chosenPutFunc) {
    getFuncPtr = chosenGetFunc;
    putFuncPtr = chosenPutFunc;
    
    root = (Cache*)malloc(sizeof(Cache));
    root->cacheCapacity = capacity;
    root->curCacheSize = 0;
    root->linkedList = createLlist();

    root->linkedList->head->next = root->linkedList->tail;
    root->linkedList->tail->prev = root->linkedList->head;

    table = (Llist**)calloc(capacity, sizeof(Llist*));
}

MyInt hash(MyInt key) {
    return key % root->cacheCapacity;
}

void insertNode(Llist *llist, Node *node) {
    Node *prevNode = llist->tail->prev;
    Node *nxtNode = llist->tail;
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

int lruGet(MyInt key) {
    int h = hash(key);
    Llist *curLlist = table[h];
    
    if (curLlist == NULL) {
        return -1;
    } 

    // printf("Item %d is already in the Cache. It will become last element\n", key);
    Node *curNode = curLlist->head->next;

    while (curNode != NULL && curNode->key != key) {
        curNode = curNode->next;
    }
    if (curNode == NULL) {
        return -1;
    }

    removeNode(curNode);
    insertNode(root->linkedList, curNode);
    return curNode->value;
}

void lruPut(MyInt key, int value) {
    int h = hash(key);
    Llist *curLlist = table[h];
    
    if (curLlist != NULL) {
        Node *curNode = curLlist->head->next;

        while (curNode != NULL && curNode->key != key) {
            curNode = curNode->next;
        }

        if (curNode != NULL) {
            curNode->value = value;

            // update lru cache
            
            removeNode(curNode);
            insertNode(root->linkedList, curNode);
            return;
        }
    } else {
        curLlist = createLlist();
        table[h] = curLlist;
    }

    Node *node = createNode(key);
    node->value = value;
    insertNode(root->linkedList, node);

    node = createNode(key);
    node->value = value;
    insertNode(curLlist, node);

    curLlist->listLength += 1;
    root->curCacheSize += 1;
    
    if (root->curCacheSize > root->cacheCapacity) {
        Node *lruNode = root->linkedList->head->next;
        printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, node->key);
        removeNode(lruNode);

        Llist *lruList = table[hash(lruNode->key)];
        Node *lruHeadNode = lruList->head->next;

        while (lruHeadNode->key != lruNode->key) {
            lruHeadNode = lruHeadNode->next;
        }
        removeNode(lruHeadNode);

        lruList->listLength -= 1;
        root->curCacheSize -= 1;

        if (lruList->listLength == 0) {
            lruList = NULL;
        }

        free(lruNode);



    // if (curLlist == NULL) {
    //     curLlist = createLlist();
        
    //     Node *node = createNode(key);
    //     node->value = value;
    //     insertNode(node);

    //     // add head and tail nodes
    //     node = createNode(key);
    //     node->value = value;
    //     Node *prevNode = curLlist->tail->prev;
    //     Node *nxtNode = curLlist->tail;
    //     prevNode->next = node;
    //     nxtNode->prev = node;
    //     node->next = nxtNode;
    //     node->prev = prevNode;

    //     // only 1 node in linkedlist
    //     // only 2 nodes
    //     // three or more nodes
    //     curLlist->listLength += 1;
    //     root->curCacheSize += 1;

    //     if (root->curCacheSize > root->cacheCapacity) {
    //         Node *lruNode = root->head->next;

    //         printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, node->key);
    //         removeNode(lruNode);

    //         Node *lruHeadNode = table[hash(lruNode->key)]->head->next;

    //         while (lruHeadNode->key != lruNode->key) {
    //             lruHeadNode = lruHeadNode->next;
    //         }
    //         removeNode(lruHeadNode);
    //         table[hash(lruNode->key)]->listLength -= 1;
    //         root->curCacheSize -= 1;

    //         if (table[hash(lruNode->key)]->listLength == 0) {
    //             table[hash(lruNode->key)] = NULL;
    //         }

    //         free(lruNode);
    //         // free(lruHeadNode);
    //     }
    // } else {
    //     Node *curNode = table[h]->head->next;
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
            
    //         Node *prevNode = curLlist->tail->prev;
    //         Node *nxtNode = curLlist->tail;
    //         prevNode->next = node;
    //         nxtNode->prev = node;
    //         node->next = nxtNode;
    //         node->prev = prevNode;

    //         curLlist->listLength += 1;
    //         root->curCacheSize += 1;
    //     }
    // }
    }
}

MyInt getRandomNum(MyInt minNum, MyInt maxNum) {
    // srand(time(0));
    return rand() % (maxNum - minNum + 1) + minNum;
}

MyInt getNextNum(MyInt num) {
    if (num % 2 == 0) {
        return num / 2;
    } else {
        return 3 * num + 1;
    }
}

int countStepsRecursive(MyInt num) {
    // base
    if (num == 1) {
        return 0;
    }

    MyInt nxtNum = getNextNum(num);

    if (nxtNum == 1) return 1;

    int steps = 1 + countStepsWrapper(nxtNum);

    return steps;
}

int countStepsWrapper(MyInt num) {
    int cachedValue = -1;
    
    if (getFuncPtr != NULL) {
        cachedValue = getFuncPtr(num);
    }

    if (cachedValue != -1) {
        printf("cache hit! num: %d\n", num);
        cacheHitCt += 1;
        return cachedValue;
    }

    int steps = countStepsRecursive(num);

    if (putFuncPtr != NULL) {
        putFuncPtr(num, steps);
    }
    return steps;
}

void freeCache() {
    Node *current = root->linkedList->head;
    while (current != NULL) {
        Node *nxt = current->next;
        free(current);
        current = nxt;
    }
    free(table);
    free(root);

    // if (cacheMap != NULL) {
    //     free(cacheMap);
    // }
}

void printCache() {
    Node *current = root->linkedList->head->next;
    while (current != root->linkedList->tail) {
        printf("key: %llu, steps: %d -> ", current->key, current->value);
        current = current->next;
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int numToTest;
    MyInt minNum;
    MyInt maxNum;
    int cacheSize;

    // printf("%d", argc);

    // for (int i = 0; i < argc; i++) {
    //     printf("%s",argv[i]);
    // } 

    // not enough arguments passed
    if (argc < 6 
        || !argv[1] 
        || 0 == sscanf(argv[1], "%d", &numToTest) 
        || numToTest < 1 
        || 0 == sscanf(argv[2], "%llu", &minNum) 
        || minNum < 1 
        || 0 == sscanf(argv[3], "%llu", &maxNum) 
        || maxNum > MY_MAX
        || !(strcmp(argv[4], "none") == 0 || strcmp(argv[4], "LRU") == 0 || strcmp(argv[4], "FIFO/Round-Robin") == 0 || strcmp(argv[4], "Round-Robin") == 0 || strcmp(argv[4], "FIFO") == 0)
        || 0 == sscanf(argv[5], "%d", &cacheSize)
        || cacheSize < 1) {
        printf("Usage: %s <positive integer - # to test> <positive integer - minimum number range > <positive integer - maximum number range> (none | LRU | FIFO/Round-Robin) <positive integer - cache size>", argv[0]);
        return 1;
    }

    if (strcmp(argv[4], "LRU") == 0) {
        initializeCache(cacheSize, 10000, lruGet, lruPut);
    } else if (strcmp(argv[4], "FIFO/Round-Robin") == 0 || strcmp(argv[4], "Round-Robin") == 0 || strcmp(argv[4], "FIFO") == 0){
        // initializeCache(cacheSize, 10000, fifoGet, fifoPut);
    } else {
        initializeCache(cacheSize, 10000, NULL, NULL);
    }

    // for (int i = 0; i < numToTest; i++) {
    //     MyInt randomNum = getRandomNum(minNum, maxNum);
    //     // MyInt randomNum = 1000;
    //     int steps = countStepsWrapper(randomNum);
    //     printf("random number: %20llu, steps: %20d\n", randomNum, steps);
    //     // printCache();
    // }

    MyInt randomNum;
    int steps;

    randomNum = 1;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printCache();
    
    randomNum = 2;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printCache();

    randomNum = 3;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printCache();

    randomNum = 4;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printCache();

    // randomNum = 5;
    // steps = countStepsWrapper(randomNum);
    // printCache();



    printf("Total Test Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", numToTest, cacheHitCt, ((float)cacheHitCt / (float)numToTest) * 100);

    freeCache();
    
    return 0;
};

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