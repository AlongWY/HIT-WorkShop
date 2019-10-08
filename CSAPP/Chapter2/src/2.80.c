//
// Created by along on 17-9-17.
//

#include <stdio.h>

/**
 * 取得x的3/4
 * @param x
 * @return （3/4）*x
 */
int threefourths(int x) {

    int a = x >> 1;
    int b = x >> 2;
    int res = a + b;

    // 当x的低两位为0x03的时候,损失会很大所以+1补充
    res += (int) ((x & 0x03) == 0x03);

    return res;
}

int main() {
    for (int i = 0; i < 33; ++i) {
        printf("%4d: %4d | %4.2lf\n", i, threefourths(i), (double) i * 3 / 4);
    }

    return 0;
}
