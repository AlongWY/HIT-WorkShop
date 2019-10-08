/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int m1, m2, m3, m4, m5, m6, m7, m8;
    int i, j, k, h;
    if (N == 32) {  // 32 x 32
        for (i = 0; i < N; i += 8) {
            for (j = 0; j < M; j += 8) {
                // 拆成8x8
                for (k = i; k < i + 8; ++k) {
                    h = j;
                    m1 = A[k][h + 0];
                    m2 = A[k][h + 1];
                    m3 = A[k][h + 2];
                    m4 = A[k][h + 3];
                    m5 = A[k][h + 4];
                    m6 = A[k][h + 5];
                    m7 = A[k][h + 6];
                    m8 = A[k][h + 7];

                    B[h + 0][k] = m1;
                    B[h + 1][k] = m2;
                    B[h + 2][k] = m3;
                    B[h + 3][k] = m4;
                    B[h + 4][k] = m5;
                    B[h + 5][k] = m6;
                    B[h + 6][k] = m7;
                    B[h + 7][k] = m8;

                }
            }
        }
    } else if (N == 64) { // 64 x 64
        for (i = 0; i < 64; i += 8) {
            for (j = 0; j < 64; j += 8) {
                // 拆成 8 x 8
                for (k = j; k < j + 4; ++k) {
                    m1 = A[k][i];
                    m2 = A[k][i + 1];
                    m3 = A[k][i + 2];
                    m4 = A[k][i + 3];
                    m5 = A[k][i + 4];
                    m6 = A[k][i + 5];
                    m7 = A[k][i + 6];
                    m8 = A[k][i + 7];

                    // 左上角复制
                    B[i][k] = m1;
                    B[i + 1][k] = m2;
                    B[i + 2][k] = m3;
                    B[i + 3][k] = m4;
                    // 右上角暂存
                    B[i][k + 4] = m5;
                    B[i + 1][k + 4] = m6;
                    B[i + 2][k + 4] = m7;
                    B[i + 3][k + 4] = m8;
                }

                for (k = i; k < i + 4; ++k) {
                    // 上一步存入到B右上角的数据
                    m1 = B[k][j + 4];
                    m2 = B[k][j + 5];
                    m3 = B[k][j + 6];
                    m4 = B[k][j + 7];
                    // A的新数据
                    m5 = A[j + 4][k];
                    m6 = A[j + 5][k];
                    m7 = A[j + 6][k];
                    m8 = A[j + 7][k];

                    B[k][j + 4] = m5;
                    B[k][j + 5] = m6;
                    B[k][j + 6] = m7;
                    B[k][j + 7] = m8;
                    B[k + 4][j] = m1;
                    B[k + 4][j + 1] = m2;
                    B[k + 4][j + 2] = m3;
                    B[k + 4][j + 3] = m4;
                }
                // 右下角的处理
                for (k = i + 4; k < i + 8; ++k) {
                    m1 = A[j + 4][k];
                    m2 = A[j + 5][k];
                    m3 = A[j + 6][k];
                    m4 = A[j + 7][k];
                    B[k][j + 4] = m1;
                    B[k][j + 5] = m2;
                    B[k][j + 6] = m3;
                    B[k][j + 7] = m4;
                }
            }
        }
    } else { // 61 x 67
        for (i = 0; i < N; i += 16) {
            for (j = 0; j < M; j += 16) {
                // 拆成 16 x 16
                for (k = i; k < i + 16 && k < N; k++) {
                    for (h = j; h < j + 16 && h < M; h++) {
                        B[h][k] = A[k][h];
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

