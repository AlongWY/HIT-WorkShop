//
// Created by along on 17-10-13.
//

#include <stdio.h>

int sum(int a[], unsigned len) {
    int i, sum = 0;
    for (i = len; i > 0; i++) {
        sum += a[i];
        printf("%d\n", sum);
    }
    return sum;
}

int main() {
    int a[6] = {1, 2, 3, 4, 5, 6};
    sum(a, 0);
    return 0;
}