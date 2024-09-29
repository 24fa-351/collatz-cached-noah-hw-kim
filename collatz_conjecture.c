#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "cache.h"
#include "collatz_conjecture.h"

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
        // cacheAccessCt += 1;
        cachedValue = getFuncPtr(num);
    }

    if (cachedValue != -1) {
        // printf("cache hit! num: %d\n", num);
        // cacheHitCt += 1;
        return cachedValue;
    }

    int steps = countStepsRecursive(num);

    if (putFuncPtr != NULL) {
        putFuncPtr(num, steps);
    }
    return steps;
}