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
int min(int a,int b){
    if (a<= b) return a;
    else return b;
}
int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M==32&&N==32){
        int a0,a1,a2,a3,a4,a5,a6,a7;
        int i,j,k;
        for (i = 0; i < 32; i += 8) {
            for (j = 0; j < 32; j += 8) {
                for (k = i; k < i + 8; k++) {
                    a0 = A[k][j];
                    a1 = A[k][j + 1];
                    a2 = A[k][j + 2];
                    a3 = A[k][j + 3];
                    a4 = A[k][j + 4];
                    a5 = A[k][j + 5];
                    a6 = A[k][j + 6];
                    a7 = A[k][j + 7];
                    B[j][k] = a0;
                    B[j + 1][k] = a1;
                    B[j + 2][k] = a2;
                    B[j + 3][k] = a3;
                    B[j + 4][k] = a4;
                    B[j + 5][k] = a5;
                    B[j + 6][k] = a6;
                    B[j + 7][k] = a7;
                }
            }
        }
    }
    if(M == 64&&N==64){
        const int len = 4;
        int a0,a1,a2,a3;
        int i,j,k,s;
        for (i = 0; i < N; i += len) {
            for (j = 0; j < N; j += len) {
                // copy
                for (k = i, s = j; k < i + len; k++, s++) {
                    a0 = A[k][j];
                    a1 = A[k][j + 1];
                    a2 = A[k][j + 2];
                    a3 = A[k][j + 3];
                    B[s][i] = a0;
                    B[s][i + 1] = a1;
                    B[s][i + 2] = a2;
                    B[s][i + 3] = a3;
                }
                // transpose
                for (k = 0; k < len; k++) {
                    for (s = k + 1; s < len; s++) {
                        a0 = B[k + j][s + i];
                        B[k + j][s + i] = B[s + j][k + i];
                        B[s + j][k + i] = a0;
                    }
                }
            }
        }
    } // for full marks there are some implementations in the internet for reference
    if(M == 61&&N==67){
        int i,j,k,s;
        int a0,a1,a2,a3,a4,a5,a6,a7;
        for (i = 0; i < N; i += 8) {
            for (j = 0; j < M; j += 23) {
                if (i + 8 <= N && j + 23 <= M) {
                    for (s = j; s < j + 23; s++) {
                        a0 = A[i][s];
                        a1 = A[i + 1][s];
                        a2 = A[i + 2][s];
                        a3 = A[i + 3][s];
                        a4 = A[i + 4][s];
                        a5 = A[i + 5][s];
                        a6 = A[i + 6][s];
                        a7 = A[i + 7][s];
                        B[s][i + 0] = a0;
                        B[s][i + 1] = a1;
                        B[s][i + 2] = a2;
                        B[s][i + 3] = a3;
                        B[s][i + 4] = a4;
                        B[s][i + 5] = a5;
                        B[s][i + 6] = a6;
                        B[s][i + 7] = a7;
                    }
                } else {
                    for (k = i; k < min(i + 8, N); k++) {
                        for (s = j; s < min(j + 23, M); s++) {
                            B[s][k] = A[k][s];
                        }
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
void trans(int M, int N, int A[N][M], int B[M][N])
{
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
void registerFunctions()
{
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
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
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

