//
// Created by along on 17-10-28.
//

#include "polynomial.h"
#include <string.h>
#include <stdlib.h>

/**
 * 进行节点的查找，使用指数进行查找
 * @param a
 * @param b
 * @return
 */
int fit(PolyNode a, PolyNode b) {
    return (a->power) < (b->power);
}

/**
 * 对节点进行插入排序
 * @param first
 * @param second
 * @return
 */
int cmp(PolyNode first, PolyNode second) {
    return (first->power) - (second->power);
}

/**
 * 跳过已经读取过的字符，使得sscanf能正确前进
 * @param str
 * @param currpos
 * @return
 */
unsigned long next_step(const char *str, unsigned long currpos) {
    for (int i = 0; i != 2 && str[currpos] != '\0';
         i += str[currpos++] == ' ' && str[currpos] != ' ');
    return currpos;
}

Poly CreatePoly(const char *polyStr) {
    //使用字符串创建一个多项式
    Poly poly = CreateList();
    int power, num;
    for (unsigned long i = 0;
         2 == sscanf(polyStr + i, "%d %d", &num, &power);
         i = next_step(polyStr, i)) {
        PolyNode polyNode = (PolyNode) malloc(sizeof(struct PolyNode));
        polyNode->num = num;
        polyNode->power = power;
        Insert(polyNode, poly, Find(polyNode, poly, fit));
    }
    return poly;
}


/**
 *
 * @param polyStr
 * @return
 */
Poly CreatePolyFromStr(const char *polyStr) {
    char polyData[STRSIZE];
    unsigned bais = 0;
    unsigned long length = strlen(polyStr);
    for (unsigned long i = 0; i != length + 1; ++i) {
        if (polyStr[i] < '9' && polyStr[i] > '0' || polyStr[i] == '\0') {
            polyData[i + bais] = polyStr[i];
        } else if (polyStr[i] == '-') {
            polyData[i + bais] = ' ';
            polyData[i + ++bais] = '-';
        } else {
            polyData[i + bais] = ' ';
        }
    }
    length = strlen(polyData);
    strcpy(polyData + length, " 0");
    printf("%s\n", polyData);
    return CreatePoly(polyData);
}


Poly Add(Poly A, Poly B) {
    Poly poly = CreateList();
    Position a = First(A);
    Position b = First(B);
    struct PolyNode pa, pb;
    // a,b有一个不空
    while (a != NULL || b != NULL) {
        if (a != NULL)pa = *(PolyNode) Retrieve(a);
        if (b != NULL)pb = *(PolyNode) Retrieve(b);
        PolyNode pc = (PolyNode) malloc(sizeof(struct PolyNode));
        // ab都不空
        // 使用归并的思路
        if ((a != NULL) && (b != NULL))
            if (pa.power > pb.power) {
                pc->power = pa.power;
                pc->num = pa.num;
                a = Advance(a);
            } else if (pa.power == pb.power) {
                pc->power = pa.power;
                pc->num = pa.num + pb.num;
                a = Advance(a);
                b = Advance(b);
            } else {
                pc->power = pb.power;
                pc->num = pb.num;
                b = Advance(b);
            }
        else if (a == NULL) {
            // a空
            pc->power = pb.power;
            pc->num = pb.num;
            b = Advance(b);
        } else {
            // b空
            pc->power = pa.power;
            pc->num = pa.num;
            a = Advance(a);
        }
        if (pc->num == 0) {
            free(pc);
        } else
            Insert(pc, poly, Find(pc, poly, fit));
    }
    return poly;
}

Poly Sub(Poly A, Poly B) {
    Poly poly = CreateList();
    Position a = First(A);
    Position b = First(B);
    struct PolyNode pa, pb;
    while (a != NULL || b != NULL) {
        if (a != NULL)pa = *(PolyNode) Retrieve(a);
        if (b != NULL)pb = *(PolyNode) Retrieve(b);
        PolyNode pc = (PolyNode) malloc(sizeof(struct PolyNode));
        pb.num = -pb.num;
        if ((a != NULL) && (b != NULL))
            if (pa.power > pb.power) {
                pc->power = pa.power;
                pc->num = pa.num;
                a = Advance(a);
            } else if (pa.power == pb.power) {
                pc->power = pa.power;
                pc->num = pa.num + pb.num;
                a = Advance(a);
                b = Advance(b);
            } else {
                pc->power = pb.power;
                pc->num = pb.num;
                b = Advance(b);
            }
        else if (a == NULL) {
            pc->power = pb.power;
            pc->num = pb.num;
            b = Advance(b);
        } else {
            pc->power = pa.power;
            pc->num = pa.num;
            a = Advance(a);
        }

        if (pc->num == 0) {
            free(pc);
        } else
            Insert(pc, poly, Find(pc, poly, fit));
    }
    return poly;
}

Poly Mult(Poly A, Poly B) {
    List poly = CreateList();
    Position a = First(A);
    PolyNode pa, pb;
    while (a != NULL) {
        Position b = First(B);
        while (b != NULL) {
            pa = Retrieve(a);
            pb = Retrieve(b);
            PolyNode pc = (PolyNode) malloc(sizeof(struct PolyNode));
            // 两个参数相乘，之后找有没有指数相同的，如果有就加里面
            pc->num = pa->num * pb->num;
            pc->power = pa->power + pb->power;
            PolyNode pd = Retrieve(Find(pc, poly, cmp));
            if (pd == NULL) {//没找到
                Insert(pc, poly, Find(pc, poly, fit));
            } else {//找到了
                pd->num += pc->num;
                if (pd->num == 0)
                    Delete(pc, poly, cmp);
                free(pc);
            }
            b = Advance(b);
        }
        a = Advance(a);
    }
    return poly;
}

void InsertPolyNode(Poly poly, PolyNode polyNode) {
    // 向多项式里面插入一个节点
    if (polyNode->num == 0) {
        free(polyNode);
    } else {
        Insert(polyNode, poly, Find(polyNode, poly, fit));
    }
}

PolyNode FirstPolyNode(Poly poly) {
    return ((PolyNode) Retrieve(First(poly)));
}

PolyPair Div(Poly A, Poly B) {
    PolyPair pair;
    /* 商：a  余：b*/
    pair.a = CreatePoly("");
    pair.b = NULL;
    PolyNode polyNode = (PolyNode) malloc(sizeof(struct PolyNode));
    //将分子的第一项除以分母的最高次项,得到首商
    polyNode->power = FirstPolyNode(A)->power - FirstPolyNode(B)->power;
    polyNode->num = FirstPolyNode(A)->num / FirstPolyNode(B)->num;
    InsertPolyNode(pair.a, polyNode);
    //将分母乘以首商，获得一个乘积
    Poly temp = Mult(B, pair.a);
    //从分子的相应项中减去刚得到的乘积
    //消去相等项，把不相等的项结合起来，得到第一余式
    pair.b = Sub(A, temp);
    DeletePoly(temp);

    while (FirstPolyNode(pair.b) != NULL &&
           FirstPolyNode(pair.a) != NULL &&
           FirstPolyNode(pair.b)->power >= FirstPolyNode(B)->power) {
        polyNode = (PolyNode) malloc(sizeof(struct PolyNode));
        //把第一余式当作新的被除式，重复前三步，得到次商与第二余式
        polyNode->power = FirstPolyNode(pair.b)->power
                          - FirstPolyNode(B)->power;
        polyNode->num = FirstPolyNode(pair.b)->num
                        / FirstPolyNode(B)->num;
        InsertPolyNode(pair.a, polyNode);
        //将分母乘以商，获得一个乘积
        temp = Mult(B, pair.a);
        DeletePoly(pair.b);
        //从分子的相应项中减去刚得到的乘积，得到新的余式
        //消去相等项，把不相等的项结合起来
        pair.b = Sub(A, temp);
        DeletePoly(temp);
    }
    return pair;
}

float Caclu(float x, Poly list) {
    float result = 0;
    for (Position pos = First(list);
         pos != NULL; pos = Advance(pos)) {
        PolyNode poly = Retrieve(pos);
        result += powf(x, poly->power) * poly->num;
    }
    return result;
}

void DeletePoly(Poly poly) {
    if (poly == NULL)
        return;
    DeleteList(poly);
}

void PolyToStr(Poly poly, char *string, size_t size) {
    if (FirstPolyNode(poly) == NULL) {
        snprintf(string, size, "0");
        return;
    }
    int strPos = 0;
    int flag = 0;
    char tempStr[STRSIZE];
    for (Position pos = First(poly);
         pos != NULL; pos = Advance(pos)) {
        PolyNode polyNode = Retrieve(pos);
        // 系数大于零且不是第一个
        if (pos != First(poly) && polyNode->num > 0) {
            snprintf(tempStr, STRSIZE, "+");
            flag = 1;
        } else
            flag = 0;
        // 系数和指数都是1
        if (polyNode->power == 1 && polyNode->num == 1) {
            snprintf(tempStr + flag, STRSIZE, "x");
        } else if (polyNode->power == 0) {
            // 指数是0
            snprintf(tempStr + flag, STRSIZE, "%d", polyNode->num);
        } else {
            // 一般情况
            snprintf(tempStr + flag, STRSIZE, "%dx^%d",
                     polyNode->num, polyNode->power);
        }
        // 将刚才的字符串写入目的字符串
        snprintf(string + strPos, size - strPos, "%s", tempStr);
        strPos += strlen(tempStr);
    }
//    snprintf(string + strPos, size - strPos, "\n");
}

void PolyDataToStr(Poly poly, char *string, size_t size) {
    int strPos = 0;
    char tempStr[STRSIZE];
    for (Position pos = First(poly);
         pos != NULL; pos = Advance(pos)) {
        PolyNode polyNode = Retrieve(pos);
        // 不是最后一个，后面加空格
        if (!IsLast(pos, poly))
            snprintf(tempStr, STRSIZE, "%d %d ",
                     polyNode->num, polyNode->power);
        else//最后一个，不加空格
            snprintf(tempStr, STRSIZE, "%d %d",
                     polyNode->num, polyNode->power);
        snprintf(string + strPos, size - strPos, "%s", tempStr);
        strPos += strlen(tempStr);
    }
//    snprintf(string + strPos, size - strPos, "\n");
}

void PolyPrint(Poly poly, const char *front,
               char *string, int flag, float x) {
    PolyToStr(poly, string, STRSIZE);
    if (flag == 0)
        printf("%4s:%20s\n", front, string);
    else
        printf("%4s:%20s\t代入x=%8.2f\t结果:%10.2f\n",
               front, string, x, Caclu(x, poly));
}
