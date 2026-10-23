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
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (N == 32)
    {
        for (int i = 0; i < N; i += 8)
        {
            for (int j = 0; j < M; j += 8)
            {//进行分块
                for (int l = 0; l < 8; l++)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        if (j + k == i + l)
                            continue;//处理对角线
                        B[j + k][i + l] = A[i + l][j + k];
                    }
                    if (i == j)
                        B[i + l][i + l] = A[i + l][i + l];//为对角线赋值
                }
            }
        }
    }
    else if (N == 64)
    { 
        int a0,a1,a2,a3,a4,a5,a6,a7;
        for(int i=0;i<N;i+=8){
            for(int j=0;j<M;j+=8){
                for(int k=0;k<4;k++){//处理一个小的4*4的矩阵,左上角
                    a0 = A[i+k][j+0];
                    a1 = A[i+k][j+1];
                    a2 = A[i+k][j+2];
                    a3 = A[i+k][j+3];
                    a4 = A[i+k][j+4];
                    a5 = A[i+k][j+5];
                    a6 = A[i+k][j+6];
                    a7 = A[i+k][j+7];//临时存储该块的全部数据
                    B[j+0][i+k] = a0;
                    B[j+1][i+k] = a1;
                    B[j+2][i+k] = a2;
                    B[j+3][i+k] = a3;//先处理了保证命中的区域Q1
                    B[j+0][i+k+4] = a4;
                    B[j+1][i+k+4] = a5;
                    B[j+2][i+k+4] = a6;
                    B[j+3][i+k+4] = a7;//将不会命中的数据Q3临时存放在已有的缓存区Q2
                }
                for(int k=0;k<4;k++){//右上和左下
                    a4 = A[i+0+4][j+k];
                    a5 = A[i+1+4][j+k];
                    a6 = A[i+2+4][j+k];
                    a7 = A[i+3+4][j+k];//得到要放进Q2的数据

                    a0 = B[j+k][i+4+0];
                    a1 = B[j+k][i+4+1];
                    a2 = B[j+k][i+4+2];
                    a3 = B[j+k][i+4+3];//得到暂存Q2的数据
                    B[j+k][i+4+0] = a4;
                    B[j+k][i+4+1] = a5;
                    B[j+k][i+4+2] = a6;
                    B[j+k][i+4+3] = a7;//解决Q2
                    B[j+k+4][i+0] = a0;
                    B[j+k+4][i+1] = a1;
                    B[j+k+4][i+2] = a2;
                    B[j+k+4][i+3] = a3;//解决Q3
                }
                for(int k=0;k<4;k++){
                    a0 = A[i+k+4][j+0+4];
                    a1 = A[i+k+4][j+1+4];
                    a2 = A[i+k+4][j+2+4];
                    a3 = A[i+k+4][j+3+4];
                    B[j+0+4][i+k+4] = a0;
                    B[j+1+4][i+k+4] = a1;
                    B[j+2+4][i+k+4] = a2;
                    B[j+3+4][i+k+4] = a3;
                }//解决Q4
        }
    }
}
    else if (N == 67)
    {
        for (int i = 0; i < N; i += 18)
        {
            for (int j = 0; j < M; j += 18)
            {
                if (j + 18 < M && i + 18 < N)
                {
                    for (int l = 0; l < 18; l++)
                    {
                        for (int k = 0; k < 18; k++)
                        {
                            if (j + k == i + l)
                                continue;//处理对角线
                            B[j + k][i + l] = A[i + l][j + k];
                        }
                        if (i == j)
                            B[i + l][i + l] = A[i + l][i + l];//对角线赋值
                    }
                }
                else if (i + 18 > N && j + 18 < M)//边界判断，之后采用原始的矩阵转置
                {
                    for (int l = 0; l + i < N; l++)
                    {
                        for (int k = 0; k < 18; k++)
                        {
                            B[j + k][i + l] = A[i + l][j + k];
                        }
                    }
                }
                else if (i + 18 < N && j + 18 > M)//边界判断，之后采用原始的矩阵转置
                {
                    for (int l = 0; l < 18; l++)
                    {
                        for (int k = 0; k + j < M; k++)
                        {
                            B[j + k][i + l] = A[i + l][j + k];
                        }
                    }
                }
                else if (i + 18 > N && j + 18 > M)//边界判断，之后采用原始的矩阵转置
                {
                    for (int l = 0; l + i < N; l++)
                    {
                        for (int k = 0; k + j < M; k++)
                        {
                            B[j + k][i + l] = A[i + l][j + k];
                        }
                    }
                }
            }
        }
    }
    return;
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

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
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

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
