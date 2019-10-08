//
// Created by along on 17-9-17.
//

#include <stdio.h>
#include <string.h>

void copy_int(int val, void *buf, size_t maxbytes) {
    // 此处判断时做了自动类型转换，无符号值总是大于等于0的
    //if (maxbytes - sizeof(val) >= 0) {

    if (maxbytes >= sizeof(val)) {
        memcpy(buf, (void *) &val, sizeof(val));
    }
}

int main() {
    int buf[32];
    memset(buf, 0, sizeof(buf));
    copy_int(1, buf, 1);
    printf("%d\n", buf[0]);

    copy_int(2, buf, 4);
    printf("%d\n", buf[0]);

    return 0;
}