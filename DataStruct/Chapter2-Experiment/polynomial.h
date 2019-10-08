//
// Created by along on 17-10-28.
//

#ifndef PROJECT_POLYNOMIAL_H
#define PROJECT_POLYNOMIAL_H

#include "list.h"
#include <malloc.h>
#include <math.h>

#define STRSIZE (50)

/**
 * 多项式节点的定义
 */
typedef struct PolyNode {
    int power;
    int num;
} *PolyNode;

typedef List Poly;

typedef struct PolyPair {
    Poly a;
    Poly b;
} PolyPair;


/**
 * 从系数，指数数据转成多项式
 * @param polyStr
 * @return
 */
Poly CreatePoly(const char *polyStr);

/**
 * 两个多项式相加
 * @param A 多项式A
 * @param B 多项式B
 * @return 和多项式
 */
Poly Add(Poly A, Poly B);

/**
 * 两个多项式相减
 * @param A 多项式A
 * @param B 多项式B
 * @return 差多项式
 */
Poly Sub(Poly A, Poly B);

/**
 * 两个多项式相乘
 * @param A 多项式A
 * @param B 多项式B
 * @return 积多项式
 */
Poly Mult(Poly A, Poly B);

/**
 * 两个多项式相除
 * @param A 多项式A
 * @param B 多项式B
 * @return  商：pair.a  余：pair.b
 */
PolyPair Div(Poly A, Poly B);

/**
 * 计算多项式
 * @param x
 * @param list
 * @return
 */
float Caclu(float x, List list);

/**
 * 将多项式转成字符串
 * @param poly
 * @param string
 * @param size
 */
void PolyToStr(Poly poly, char *string, size_t size);

/**
 * 删除一个多项式
 * @param poly
 */
void DeletePoly(Poly poly);

/**
 * 将多项式的系数指数数据存入字符串
 * @param poly 多项式
 * @param string 存入的字符串地址
 * @param size 字符串内存大小
 */
void PolyDataToStr(Poly poly, char *string, size_t size);

/**
 * 从字符串产生多项式
 * @param polyStr
 * @return
 */
Poly CreatePolyFromStr(const char *polyStr);

/**
 * 打印多项式
 * @param poly 要打印的多项式
 * @param front 多项式前缀
 * @param string 缓冲区
 * @param flag 是否代入x进行计算
 * @param x 代入的x
 */
void PolyPrint(Poly poly, const char *front,
               char *string, int flag, float x);

#endif //PROJECT_POLYNOMIAL_H
