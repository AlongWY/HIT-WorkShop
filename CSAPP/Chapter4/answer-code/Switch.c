#include <stdio.h>

long switchv(long idx) {
    long result = 0;
    switch (idx) {
    case 0:result = 0xaaa;
        break;
    case 2:
    case 5:result = 0xbbb;
        break;
    case 3:result = 0xccc;
        break;
    default:result = 0xddd;
    }
    return result;
}

/** 测试数据 */
#define CNT 8
#define MINVAL -1

int main() {
    long vals[CNT];
    int i;
    for (i = 0; i < cnt; ++i) {
        vals[i] = switchv(i + MINVAL);
        printf("idx = %ld,val = 0x%lx\n", i + MINVAL, vals[i]);
    }
    return 0;
}