//
// Created by along on 17-9-17.
//
#include <stdio.h>

typedef unsigned float_bits;

float U2F_BIT(unsigned U) {
    return *((float *) &U);
}

/**
 * 对于浮点数 f，这个函数返回 -f;如果 f 是 NaN，这个函数简单的返回 f。
 * @param f
 * @return
 */
float_bits float_negate(float_bits f);

/*
 * 输出结果
 * NOT EQUAL | Bits:7fff0000 | Machine:-nan | Mine:nan
 * NOT EQUAL | Bits:7fff0001 | Machine:-nan | Mine:nan
 * NOT EQUAL | Bits:7fff0002 | Machine:-nan | Mine:nan
 * .................
 * NOT EQUAL | Bits:7ffffffe | Machine:-nan | Mine:nan
 * NOT EQUAL | Bits:7fffffff | Machine:-nan | Mine:nan
 * NOT EQUAL | Bits:ff800001 | Machine:nan | Mine:-nan
 * NOT EQUAL | Bits:ff800002 | Machine:nan | Mine:-nan
 * .................
 * NOT EQUAL | Bits:fffffffd | Machine:nan | Mine:-nan
 * NOT EQUAL | Bits:fffffffe | Machine:nan | Mine:-nan
 * NOT EQUAL | Bits:ffffffff | Machine:nan | Mine:-nan
 */

int main() {
    for (float_bits i = 0;; ++i) {

        float Machine = -U2F_BIT(i);
        float Mine = U2F_BIT(float_negate(i));

        if (Machine != Mine)
            printf("NOT EQUAL | Bits:%x | Machine:%f | Mine:%f\n", i, Machine, Mine);

        if (i == 0xFFFFFFFF)
            break;
    }

    return 0;
}

float_bits float_negate(float_bits f) {
    unsigned sign = f >> 31;
    unsigned exp = f >> 23 & 0xFF;
    unsigned frac = f & 0x7FFFFF;

    if ((exp == 0xFF) && (frac != 0)) {
        return f;
    } else {
        return ((~sign) << 31) + (exp << 23) + frac;
    }
}
