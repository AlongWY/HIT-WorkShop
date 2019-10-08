//
// Created by along on 17-9-17.
//


#include <stdio.h>

/**
 * 替换x的第i块内存为b
 * replace_byte(0x12345678, 2, 0xAB) --> 0x12AB5678
 * replace_byte(0x12345678, 0, 0xAB) --> 0x123456AB
 * @param x
 * @param i
 * @param b
 * @return
 */
unsigned replace_byte(unsigned x, int i, unsigned char b);

int main() {
    printf("%x", replace_byte(0x12345678, 1, 0xAB));
    return 0;
}

unsigned replace_byte(unsigned x, int i, unsigned char b) {
    unsigned char *uptr = (unsigned char *) &x;
    *(uptr + i) = b;
    return x;
}