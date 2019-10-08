//
// Created by along on 17-10-24.
//
#include <string.h>
#include <stdio.h>

/**
 * 递归算阶乘
 * @param num
 * @return
 */
int Fact(int num) {
    if (num > 1)
        return num * Fact(num - 1);
    return 1;
}

struct Bomb {
    int level;
    int achieve;
    struct Bomb *next;
};

int main() {
    int test = 0;
    int *ptr = &test;
    struct Bomb bomb;
    bomb.level = 5;
    bomb.achieve = 0;
    for (int i = 0; i != 5; ++i) {
        if (i % 2 == 0) {
            test = i;
            bomb.next = NULL;
        } else {
            *ptr = i;
            bomb.achieve++;
        }
        printf("%d", *ptr);
    }

    switch (test) {
        case 0:
            printf("0\n");
            break;
        case 4:
            printf("%d\n", Fact(test));
        default:
            break;
    }


    return 0;
}