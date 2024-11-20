#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "collatz_conjecture.h"

void simpleTest()
{
    // simple test for LRU and FIFO
    MyInt randomNum;
    int steps;

    randomNum = 1;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printf("random number: %20llu, steps: %20d\n", randomNum, steps);
    printf("Cache Access Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", cacheAccessCt, cacheHitCt, ((float)cacheHitCt / (float)cacheAccessCt) * 100);
    printCache();

    randomNum = 2;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printf("random number: %20llu, steps: %20d\n", randomNum, steps);
    printf("Cache Access Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", cacheAccessCt, cacheHitCt, ((float)cacheHitCt / (float)cacheAccessCt) * 100);
    printCache();

    randomNum = 3;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printf("random number: %20llu, steps: %20d\n", randomNum, steps);
    printf("Cache Access Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", cacheAccessCt, cacheHitCt, ((float)cacheHitCt / (float)cacheAccessCt) * 100);
    printCache();

    randomNum = 5;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printf("random number: %20llu, steps: %20d\n", randomNum, steps);
    printf("Cache Access Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", cacheAccessCt, cacheHitCt, ((float)cacheHitCt / (float)cacheAccessCt) * 100);
    printCache();

    randomNum = 2;
    printf("%d\n", randomNum);
    steps = countStepsWrapper(randomNum);
    printf("random number: %20llu, steps: %20d\n", randomNum, steps);
    printf("Cache Access Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", cacheAccessCt, cacheHitCt, ((float)cacheHitCt / (float)cacheAccessCt) * 100);
    printCache();
}

int main(int argc, char *argv[])
{
    int numToTest;
    MyInt minNum;
    MyInt maxNum;
    int cacheSize;

    // not enough arguments passed
    if (argc < 6 || !argv[1] || 0 == sscanf(argv[1], "%d", &numToTest) || numToTest < 1 || 0 == sscanf(argv[2], "%llu", &minNum) || minNum < 1 || 0 == sscanf(argv[3], "%llu", &maxNum) || maxNum > MY_MAX || !(strcmp(argv[4], "none") == 0 || strcmp(argv[4], "LRU") == 0 || strcmp(argv[4], "FIFO/Round-Robin") == 0 || strcmp(argv[4], "Round-Robin") == 0 || strcmp(argv[4], "FIFO") == 0) || 0 == sscanf(argv[5], "%d", &cacheSize) || cacheSize < 1)
    {
        printf("Usage: %s <positive integer - # to test> <positive integer - minimum number range > <positive integer - maximum number range> (none | LRU | FIFO/Round-Robin) <positive integer - cache size>", argv[0]);
        return 1;
    }

    if (strcmp(argv[4], "LRU") == 0)
    {
        initializeCache(cacheSize, 10000, naiveLRUGet, naiveLRUPut);
    }
    else if (strcmp(argv[4], "FIFO/Round-Robin") == 0 || strcmp(argv[4], "Round-Robin") == 0 || strcmp(argv[4], "FIFO") == 0)
    {
        initializeCache(cacheSize, 10000, naiveFIFOGet, naiveFIFOPut);
    }
    else
    {
        initializeCache(cacheSize, 10000, NULL, NULL);
    }

    for (int i = 0; i < numToTest; i++)
    {
        MyInt randomNum = getRandomNum(minNum, maxNum);
        int steps = countStepsWrapper(randomNum);
        // printf("random number: %20llu, steps: %20d\n", randomNum, steps);
        // printf("Cache Access Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", cacheAccessCt, cacheHitCt, ((float)cacheHitCt / (float)cacheAccessCt) * 100);
        // printCache();
    }

    // prevent divide by 0
    if (cacheAccessCt == 0)
    {
        cacheAccessCt = 1;
    }

    printf("Cache Access Ct: %d, Cache Hit Ct: %d, Cache Hit Rate: %.2f%%\n", cacheAccessCt, cacheHitCt, ((float)cacheHitCt / (float)cacheAccessCt) * 100);

    freeCache();

    return 0;
};