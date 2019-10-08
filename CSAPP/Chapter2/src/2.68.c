//
// Created by along on 17-9-17.
//

#include <stdio.h>
#include <limits.h>

/**
 * 从低位向高位变1
 * @param n
 * @return
 */
int lower_one_mask(int n) {
    // 使用算数移位再取反
    return ~(INT_MIN >> (((sizeof(int) << 3) - 1) - n));
}

int main() {
    printf("%x\n", lower_one_mask(17));
    return 0;
}
