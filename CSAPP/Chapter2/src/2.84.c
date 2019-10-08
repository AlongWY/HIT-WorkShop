//
// Created by along on 17-9-17.
//

#include <stdio.h>
#include <limits.h>

unsigned int f2u(float f) {
    return *((unsigned *) (&f));
}

/**
 * 返回 x<=y
 * @param x
 * @param y
 * @return
 */
int float_le(float x, float y) {
    unsigned ux = f2u(x);
    unsigned uy = f2u(y);

    // 获取符号位
    unsigned sx = ux >> 31;
    unsigned sy = uy >> 31;

    // 1)位级整数编码规则
    // 比较下x,y的大小（仅使用ux,uy,sx,sy)
//    return ((ux ^ uy) == 0) ||                                            // x,y完全相等
//           (((ux & INT_MAX) == 0) && ((uy & INT_MAX) == 0)) ||            // 去除最高位x,y等于0
//           ((sx == 1) && (sy == 0)) ||                                    // x为负，y为正并且都不是0
//           ((sx == 0) && (sy == 0) && ((ux - uy) >> 31 == 1)) ||          // 符号同正 ux < uy
//           ((sx == 1) && (sy == 1) && ((ux - uy) >> 31 == 0));            // 符号同负 ux > uy

    // 2)无限制
    return ((ux ^ uy) == 0) ||                                            // x,y完全相等
           (((ux & INT_MAX) == 0) && ((uy & INT_MAX) == 0)) ||            // 去除最高位x,y等于0
           ((sx == 1) && (sy == 0)) ||                                    // x为负，y为正并且都不是0
           ((sx == 0) && (sy == 0) && ((ux < uy))) ||                     // 符号同正 ux < uy
           ((sx == 1) && (sy == 1) && ((ux > uy)));                       // 符号同负 ux > uy

}

int main() {
    printf("%x", float_le(1.0f, 2.0f));
    return 0;
}