//
// Created by along on 17-9-17.
//

#include <stdio.h>
//#include <glob.h>

typedef unsigned char *byte_pointer;


/**
 * 显示内存数据
 * @param start
 * @param len
 */
void show_bytes(byte_pointer start, size_t len) {
    size_t i;
    for (i = 0; i < len; ++i)
        printf(" %.2x", start[i]);
    printf("\n");
}

/**
 * 查看int在内存中的存储情况
 * @param x
 */
void show_int(int x) {
    show_bytes((byte_pointer) &x, sizeof(int));
}

/**
 * 查看float在内存中的存储情况
 * @param x
 */
void show_float(float x) {
    show_bytes((byte_pointer) &x, sizeof(float));
}

/**
 * 查看double在内存中的存储情况
 * @param x
 */
void show_double(double x) {
    show_bytes((byte_pointer) &x, sizeof(double));
}

int main() {
    // 输出 ： 78 56 34 12
    show_int(0x12345678);
    return 0;
}
