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
  if (M == 32) {
    // basic block technique: 8 * 8 block
    int i, j, k, v1, v2, v3, v4, v5, v6, v7, v8;
    for (i = 0; i < 32; i += 8)
      for (j = 0; j < 32; j += 8)
        for (k = i; k < (i + 8); ++k) {
          v1 = A[k][j];
          v2 = A[k][j + 1];
          v3 = A[k][j + 2];
          v4 = A[k][j + 3];
          v5 = A[k][j + 4];
          v6 = A[k][j + 5];
          v7 = A[k][j + 6];
          v8 = A[k][j + 7];
          B[j][k] = v1;
          B[j + 1][k] = v2;
          B[j + 2][k] = v3;
          B[j + 3][k] = v4;
          B[j + 4][k] = v5;
          B[j + 5][k] = v6;
          B[j + 6][k] = v7;
          B[j + 7][k] = v8;
        }
  } else if (M == 64) {
    int i, j, k, v1, v2, v3, v4;
    for (i = 0; i < M; i += 4)
      for (j = 0; j < M; j += 4)
        for (k = i; k < (i + 4); ++k) {
          v1 = A[k][j];
          v2 = A[k][j + 1];
          v3 = A[k][j + 2];
          v4 = A[k][j + 3];
          B[j][k] = v1;
          B[j + 1][k] = v2;
          B[j + 2][k] = v3;
          B[j + 3][k] = v4;
        }
  } else {
    int i, j, k, l;
    for (i = 0; i < N; i += 17) {
      for (j = 0; j < M; j += 17) {
        for (k = i; k < i + 17 && k < N; k++) {
          for (l = j; l < j + 17 && l < M; l++) {
            B[l][k] = A[k][l];
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
