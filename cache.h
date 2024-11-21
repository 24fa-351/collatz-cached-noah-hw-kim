#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MY_MAX ULLONG_MAX

typedef unsigned long long MyInt;

typedef int (*getFunc)(MyInt);
typedef void (*putFunc)(MyInt, int);

typedef struct node
{
    MyInt key;
    int value;
    struct node *next;
    struct node *prev;
} Node;

typedef struct cache
{
    int cacheCapacity;
    int curCacheSize;
    Node *head;
    Node *tail;
} Cache;

extern getFunc getFuncPtr;
extern putFunc putFuncPtr;
extern Cache *root;
extern int cacheHitCt;
extern int cacheAccessCt;

Node *createNode(MyInt num);
void initializeCache(int capacity, MyInt maxKeys, getFunc chosenGetFunc, putFunc chosenPutFunc);
void insertNode(Node *node);
void removeNode(Node *node);
int naiveLRUGet(MyInt key);
void naiveLRUPut(MyInt key, int value);
int naiveFIFOGet(MyInt key);
void naiveFIFOPut(MyInt key, int value);
void freeCache();
void printCache();

#endif