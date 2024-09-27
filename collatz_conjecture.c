#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MY_MAX ULLONG_MAX
#define MAX_KEYS 10000

typedef struct node
{
    int key;
    int value;
    struct node* next;
    struct node* prev;
} Node;

typedef struct cache
{
    int cacheCapacity;
    int curCacheSize;
    struct node* head;
    struct node* tail;
} Cache;

typedef int (*getFunc)(int);
typedef void (*putFunc)(int, int);
getFunc getFuncPtr;
putFunc putFuncPtr;

int getRandomNum(int minNum, int maxNum);
int countSteps(int randomNum);
int getNextNum(int num);
void initializeCache(int capacity, getFunc chosenFunc);

Cache *root;
Node *cacheMap[MAX_KEYS];

Node *createNode(int num) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->key = num;
    node->value = -1;

    node->prev = NULL;
    node->next = NULL;

    return node;
}

void initializeCache(int capacity, getFunc chosenGetFunc, putFunc chosenPutFunc) {
    getFuncPtr = chosenGetFunc;
    putFuncPtr = chosenPutFunc;
    
    root = (Cache*)malloc(sizeof(Cache));
    root->cacheCapacity = capacity;
    root->curCacheSize = 0;
    root->head = createNode(-1);
    root->tail = createNode(-1);

    root->head->next = root->tail;
    root->tail->prev = root->head;

    for (int i = 0; i < MAX_KEYS; i++) {
        cacheMap[i] = NULL;
    }
}

void lruInsert(Node *node) {
    Node *prevNode = root->tail->prev;
    Node *nxtNode = root->tail;
    prevNode->next = node;
    nxtNode->prev = node;
    node->next = nxtNode;
    node->prev = prevNode;

    cacheMap[node->key] = node;
}

void lruRemove(Node *node) {
    Node *prevNode = node->prev;
    Node *nxtNode = node->next;
    prevNode->next = nxtNode;
    nxtNode->prev = prevNode;
}

int lruGet(int key) {
    Node *curNode = cacheMap[key];
    
    if (curNode != NULL) {
        lruRemove(curNode);
        lruInsert(curNode);
        return curNode->value;
    }
    return -1;
}

void lruPut(int key, int value) {
    Node *curNode = cacheMap[key];
    
    if (curNode != NULL) {
        lruRemove(curNode);
    }
    Node *node = createNode(key);
    node->value = value;
    lruInsert(node);
    root->curCacheSize += 1;

    if (root->curCacheSize > root->cacheCapacity) {
        Node *lruNode = root->head->next;
        lruRemove(lruNode);
        cacheMap[lruNode->key] = NULL;
        free(lruNode);
        root->curCacheSize -= 1;
    }
}

int getRandomNum(int minNum, int maxNum) {
    // srand(time(0));
    int randomNum = rand() % (maxNum - minNum + 1) + minNum;
    return randomNum;
}

int countStepsWrapper(int num) {
    if (getFuncPtr(num) != -1) {
        return getFuncPtr(num);
    }
    
    ptr(num);
    return countSteps(num);
}

int countSteps(int randomNum) {
    int count = 0;
    int curNum = randomNum;
    // printf("current number: %d, current count: %d\n", curNum, count);

    while (curNum != 1) {
        count += 1;
        curNum = getNextNum(curNum);
        // printf("current number: %d, current count: %d\n", curNum, count);
    }
    // printf("\n");
    return count;
}

int getNextNum(int num) {
    if (num % 2 == 0) {
        return num / 2;
    } else {
        return 3 * num + 1;
    }
}

int main(int argc, char *argv[]) {
    int numToTest;
    int minNum;
    int maxNum;
    int cacheSize;

    // not enough arguments passed
    if (argc < 5 
        || !argv[1] 
        || 0 == sscanf(argv[1], "%d", &numToTest) 
        || numToTest < 1 
        || 0 == sscanf(argv[2], "%d", &minNum) 
        || minNum < 1 
        || 0 == sscanf(argv[3], "%d", &maxNum) 
        || maxNum > MY_MAX 
        || !(strcmp(argv[4], "none") == 0 || strcmp(argv[4], "LRU") == 0 || strcmp(argv[4], "FIFO/Round-Robin") == 0)
        || 0 == sscanf(argv[5], "%d", &cacheSize)
        || cacheSize < 1) {
        printf("Usage: %s <positive integer - # to test> <positive integer - minimum number range > <positive integer - maximum number range> (none | LRU | FIFO/Round-Robin) <positive integer - cache size>", argv[0]);
        return 1;
    }

    if (strcmp(argv[4], "LRU") == 0) {
        initializeCache(cacheSize, lruGet, lruPut);
    } else {
        initializeCache(cacheSize, NULL, NULL);
    }
    

    for (int i = 0; i < numToTest; i++) {
        int randomNum = getRandomNum(minNum, maxNum);
        int steps = countSteps(randomNum);
        printf("random number: %20d, steps: %20d\n", randomNum, steps);
    }

    return 0;
};