//
// Created by along on 17-10-24.
//

#include <stdio.h>
#include "list.h"
#include "polynomial.h"
#include <string.h>

int main(int argc, char **argv) {
    FILE *fileOut = NULL;
    Poly first = NULL;
    Poly second = NULL;
    Poly polyOut = NULL;
    PolyPair pair;
    pair.a = pair.b = NULL;
    char str[STRSIZE];
    int flag = 0;//是否进行运算
    int op = '+';
    float x = 0;
    if (argc != 1) {
        for (int i = 1; i != argc; ++i) {

            if (0 == strcmp(argv[i], "-o")) {
                //输出文件
                fileOut = fopen(argv[++i], "w");
                if (fileOut == NULL) {
                    printf("Can't open the file:%s", argv[i]);
                }
            } else if (0 == strncmp(argv[i], "-poly", 5)) {
                //多项式输入
                if (strlen(argv[i]) == 5) {
                    first = CreatePoly(argv[++i]);
                    second = CreatePoly(argv[++i]);
                } else {
                    first = CreatePolyFromStr(argv[++i]);
                    second = CreatePolyFromStr(argv[++i]);
                }
            } else if (0 == strncmp(argv[i], "-p", 2)) {
                //加减乘除 + - * /
                op = argv[i][2];
            } else if (0 == strcmp(argv[i], "-i")) {
                FILE *fileIn = fopen(argv[++i], "r");
                if (fileIn == NULL) {
                    printf("Can't open the file:%s\n", argv[i]);
                }
                fgets(str, STRSIZE, fileIn);
                first = CreatePoly(str);
                fgets(str, STRSIZE, fileIn);
                second = CreatePoly(str);
                fclose(fileIn);
            } else if (0 == strcmp(argv[i], "-x")) {
                if (sscanf(argv[++i], "%f", &x) == 1)
                    flag = 1;
            }
        }
    } else {
        printf("Usage:\n"\
        "\t-o    filename\n"\
        "\t-poly \"poly-a\" \"poly-b\"\n"\
        "\t-i    filename\n"\
        "\t-p    [add,sub,mult,div]\n"\
        "\t-x    a num\n"\
        "多项式输入如下: 5*x^4+3*x^2+1 -> 5 4 3 2 1 0\n"
        );
        return 0;
    }
    PolyPrint(first, "一", str, flag, x);
    PolyPrint(second, "二", str, flag, x);
    switch (op) {
        default:
        case 'a':
            polyOut = Add(first, second);
            PolyPrint(polyOut, "和", str, flag, x);
            break;
        case 's':
            polyOut = Sub(first, second);
            PolyPrint(polyOut, "差", str, flag, x);
            break;
        case 'm':
            polyOut = Mult(first, second);
            PolyPrint(polyOut, "积", str, flag, x);
            break;
        case 'd':
            pair = Div(first, second);
            PolyPrint(pair.a, "商", str, flag, x);
            PolyPrint(pair.b, "余", str, flag, x);
            break;
    }
    if (fileOut != NULL && op != 'd') {
        if (polyOut != 0) {
            PolyDataToStr(polyOut, str, STRSIZE);
            fprintf(fileOut, "%s\n", str);
        } else if (pair.a == NULL) {
            PolyDataToStr(pair.a, str, STRSIZE);
            fprintf(fileOut, "%s\n", str);
            PolyDataToStr(pair.b, str, STRSIZE);
            fprintf(fileOut, "%s\n", str);
        }
        fclose(fileOut);
    }
    DeletePoly(pair.a);
    DeletePoly(pair.b);
    DeletePoly(polyOut);
    DeletePoly(first);
    DeletePoly(second);
    return 0;
}


