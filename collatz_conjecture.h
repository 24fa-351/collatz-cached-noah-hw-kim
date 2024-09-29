#ifndef COLLATZ_H
#define COLLATZ_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long MyInt;

MyInt getRandomNum(MyInt minNum, MyInt maxNum);
MyInt getNextNum(MyInt num);
int countStepsRecursive(MyInt num);
int countStepsWrapper(MyInt num);

#endif