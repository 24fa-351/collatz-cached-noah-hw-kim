#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

#define MY_MAX ULLONG_MAX
#define HASH_SIZE 1024

typedef unsigned long long MyInt;

getFunc getFuncPtr;
putFunc putFuncPtr;
Cache *root;
int cacheHitCt;
int cacheAccessCt;

Node *createNode(MyInt num)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->key = num;
    node->value = -1;

    node->prev = NULL;
    node->next = NULL;

    return node;
}

void initializeCache(int capacity, MyInt maxKeys, getFunc chosenGetFunc, putFunc chosenPutFunc)
{
    getFuncPtr = chosenGetFunc;
    putFuncPtr = chosenPutFunc;

    root = (Cache *)malloc(sizeof(Cache));
    root->cacheCapacity = capacity;
    root->curCacheSize = 0;
    root->head = createNode(0);
    root->tail = createNode(0);

    root->head->next = root->tail;
    root->tail->prev = root->head;

    cacheHitCt = 0;
    cacheAccessCt = 0;
}

void insertNode(Node *node)
{
    Node *prevNode = root->tail->prev;
    Node *nxtNode = root->tail;
    prevNode->next = node;
    nxtNode->prev = node;
    node->next = nxtNode;
    node->prev = prevNode;
}

void removeNode(Node *node)
{
    Node *prevNode = node->prev;
    Node *nxtNode = node->next;
    prevNode->next = nxtNode;
    nxtNode->prev = prevNode;
}

int naiveLRUGet(MyInt key)
{
    cacheAccessCt++;

    Node *curNode = root->head->next;

    while (curNode != NULL && curNode->key != key)
    {
        curNode = curNode->next;
    }

    if (curNode == NULL)
    {
        return -1;
    }

    cacheHitCt++;
    removeNode(curNode);
    insertNode(curNode);
    return curNode->value;
}

void naiveLRUPut(MyInt key, int value)
{
    Node *node = createNode(key);
    node->value = value;
    insertNode(node);
    root->curCacheSize += 1;

    if (root->curCacheSize > root->cacheCapacity)
    {
        Node *lruNode = root->head->next;
        // printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, node->key);
        removeNode(lruNode);
        root->curCacheSize -= 1;
        free(lruNode);
    }
}

int naiveFIFOGet(MyInt key)
{
    cacheAccessCt++;

    Node *curNode = root->head->next;

    while (curNode != NULL && curNode->key != key)
    {
        curNode = curNode->next;
    }

    if (curNode == NULL)
    {
        return -1;
    }

    cacheHitCt++;

    return curNode->value;
}

void naiveFIFOPut(MyInt key, int value)
{
    Node *node = createNode(key);
    node->value = value;
    insertNode(node);
    root->curCacheSize += 1;

    if (root->curCacheSize > root->cacheCapacity)
    {
        Node *lruNode = root->head->next;
        // printf("Cache memory is full! %llu will be replaced by %llu\n", lruNode->key, node->key);
        removeNode(lruNode);
        root->curCacheSize -= 1;
        free(lruNode);
    }
}

void freeCache()
{
    Node *current = root->head;
    while (current != NULL)
    {
        Node *nxt = current->next;
        free(current);
        current = nxt;
    }

    free(root);
    root = NULL;
}

void printCache()
{
    Node *current = root->head->next;
    while (current != root->tail)
    {
        printf("key: %llu, steps: %d\n", current->key, current->value);
        current = current->next;
    }
    printf("\n");
}
