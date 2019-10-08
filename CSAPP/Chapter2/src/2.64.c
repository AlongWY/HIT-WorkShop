//
// Created by along on 17-9-17.
//

#include <stdio.h>

/**
 * 如果x的任何一个奇数位为1则返回1,否则返回零
 * @param x
 * @return
 */
int any_odd_one(unsigned x) {
    return (int) ((x & 0x55555555) != 0);
}

int main() {
    for (unsigned i = 0; i < 10; ++i) {
        printf("Bits:%x Key:%ud IsOdd:%d\n", i, i, any_odd_one(i));
    }
    return 0;
}
