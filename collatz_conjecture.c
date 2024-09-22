#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int countSteps(int randomNum);
int getNextNum(int num);
int getRandomNum(int minNum, int maxNum);

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

int getRandomNum(int minNum, int maxNum) {
    // srand(time(0));
    int randomNum = rand() % (maxNum - minNum + 1) + minNum;
    return randomNum;
}

int main(int argc, char *argv[]) {
    // not enough arguments passed
    if (argc < 4) {
        printf("not enough numbers entered\n");
        printf("please enter number of values to test, smallest value to test, and largest value to test\n");
        return 0;
    }
    
    int numToTest = atoi(argv[1]);
    int minNum = atoi(argv[2]);
    int maxNum = atoi(argv[3]);

    for (int i = 0; i < numToTest; i++) {
        int randomNum = getRandomNum(minNum, maxNum);
        int steps = countSteps(randomNum);
        printf("random number: %d, steps: %d\n", randomNum, steps);
    }

    return 0;
};