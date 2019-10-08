//
// Created by along on 17-10-10.
// 包含C各种数据类型，包括指针、数组、struct、
// union、enum、函数main的地址、printf的地址等。
// 打印每个变量名、内容、地址、对应16进制的内存各字节
//

#include <stdio.h>
#include <string.h>

typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start, size_t len) {
    size_t i;
    for (i = 0; i < len; ++i)
        printf(" %.2x", start[i]);
    printf("\n");
}

char *mstring = "冯云龙";
int intenger = 1160300202;
long lintenger = 1160300202;
long long llintenger = 1160300202;
float floatnum = 411627199805247416.f;
double doublenum = 411627199805247416;
int *pInt = &intenger;
int intArray[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
struct StructData {
    int aInt;
    char aChar;
} structData;
union UnionData {
    unsigned uData;
    float floatData;
} unionData;
enum {
    Red, Yellow, Blue
} enumData = Blue;

//变量名、变量值、变量地址、变量对应16进制的内存各字节。
int main() {
    printf("变量名:mstring     变量值:%s   变量地址:%p  内存:",
           mstring, &mstring);
    show_bytes((byte_pointer) &mstring, strlen(mstring));
    printf("变量名:intenger    变量值:%d   变量地址:%p  内存:",
           intenger, &intenger);
    show_bytes((byte_pointer) &intenger, sizeof(intenger));
    printf("变量名:lintenger   变量值:%ld  变量地址:%p  内存:",
           lintenger, &lintenger);
    show_bytes((byte_pointer) &lintenger, sizeof(lintenger));
    printf("变量名:llintenger  变量值:%lld 变量地址:%p  内存:",
           llintenger, &llintenger);
    show_bytes((byte_pointer) &llintenger, sizeof(llintenger));
    printf("变量名:floatnum    变量值:%f   变量地址:%p  内存:",
           floatnum, &floatnum);
    show_bytes((byte_pointer) &floatnum, sizeof(floatnum));
    printf("变量名:doublenum   变量值:%lf  变量地址:%p  内存:",
           doublenum, &doublenum);
    show_bytes((byte_pointer) &doublenum, sizeof(doublenum));
    printf("变量名:pInt        变量值:%p  变量地址:%p  内存:",
           pInt, &pInt);
    show_bytes((byte_pointer) &pInt, sizeof(pInt));
    printf("变量名:intArray    变量值:%p  变量地址:%p  内存:",
           intArray, &intArray);
    show_bytes((byte_pointer) &intArray, sizeof(intArray));
    structData.aChar = 'a';
    structData.aInt = 1;
    printf("变量名:structData  变量值:|aChar:%c|aInt:%d|  "
                   "变量地址:%p  内存:",
           structData.aChar, structData.aInt, &structData);
    show_bytes((byte_pointer) &structData, sizeof(structData));
    unionData.uData = 0x12345678;
    printf("变量名:unionData   变量值:|uData:%u|floatData:%f|  "
                   "变量地址:%p  内存:",
           unionData.uData, unionData.floatData, &unionData);
    show_bytes((byte_pointer) &unionData, sizeof(unionData));
    printf("变量名:enumData    变量值:%d  变量地址:%p  内存:",
           enumData, &enumData);
    show_bytes((byte_pointer) &enumData, sizeof(enumData));
    printf("函数名:main        函数地址:%p\n", main);
    printf("函数名:printf      函数地址:%p\n", printf);
    return 0;
}