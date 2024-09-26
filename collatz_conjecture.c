#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MY_MAX ULLONG_MAX

typedef struct node
{
    int num;
    struct node* next;
    struct node* prev;
} Node;

typedef struct cache
{
    int capacity;
    // Pointer to the next node
    struct node* head;
    // Pointer to the previous node
    struct node* tail;
} Cache;

typedef int (*fibFunc)(int);
fibFunc ptr;

int getRandomNum(int minNum, int maxNum);
int countSteps(int randomNum);
int getNextNum(int num);
int cacheSize;
Cache *root;
void initializeCache(int capacity, fibFunc chosenFunc);

void initializeCache(int capacity, fibFunc chosenFunc) {
    root = (Cache*)malloc(sizeof(Cache));
    root->capacity = capacity;
    root->head = createNode(-1);
    root->tail = createNode(-1);

    root->head->prev = root->tail;
    root->tail->next = root->head;
}

Node *createNode(int num) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->num = num;

    node->prev = NULL;
    node->next = NULL;

    return node;
}

int lruGet(int num) {
    return -1;
}

int lruPut(int num) {
    return -1;
}



int getRandomNum(int minNum, int maxNum) {
    // srand(time(0));
    int randomNum = rand() % (maxNum - minNum + 1) + minNum;
    return randomNum;
}

int countStepsWrapper(int num) {
    if (ptr(num)) {
        return ptr(num);
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

    // if (strcmp(argv[4], "LRU") == 0) {
    //     initializeCache(lruGet, lruPut);
    // } else if (strcmp(argv[4], "FIFO/Round-Robin") == 0)
    // {
    //     initializeCache(fifoGet, fifoPut);
    // }
    

    for (int i = 0; i < numToTest; i++) {
        int randomNum = getRandomNum(minNum, maxNum);
        int steps = countSteps(randomNum);
        printf("random number: %20d, steps: %20d\n", randomNum, steps);
    }

    return 0;
};