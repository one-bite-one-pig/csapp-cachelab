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
	int a,b,c,d,e,f,g,h;
    if (M == 32) {
        for(int i = 0; i < 32; i += 8)
        for(int j = 0; j < 32; j += 8)
            for (int k = i; k < i + 8; k++)
            {
                int a_0 = A[k][j];
                int a_1 = A[k][j+1];
                int a_2 = A[k][j+2];
                int a_3 = A[k][j+3];
                int a_4 = A[k][j+4];
                int a_5 = A[k][j+5];
                int a_6 = A[k][j+6];
                int a_7 = A[k][j+7];
                B[j][k] = a_0;
                B[j+1][k] = a_1;
                B[j+2][k] = a_2;
                B[j+3][k] = a_3;
                B[j+4][k] = a_4;
                B[j+5][k] = a_5;
                B[j+6][k] = a_6;
                B[j+7][k] = a_7;
            } 
    }
	
    else if (M == 64) {
    for (int i = 0; i < N; i += 8) { // 当前行
        for (int j = 0; j < M; j += 8) { // 当前列
            // 先复制
            for (int k = 0;k < 4;++k) {
                // 从 A 中取 α[0][0]+α[0][1]的第 k 行，执行 4 次，即读出 A 的上半块，每次循环，读 A 都必然 MISS
                // 所以总计 4 次 MISS。
                a = A[i + k][j];
                b = A[i + k][j + 1];
                c = A[i + k][j + 2];
                d = A[i + k][j + 3];
                e = A[i + k][j + 4];
                f = A[i + k][j + 5];
                g = A[i + k][j + 6];
                h = A[i + k][j + 7];
                // 转置前四个数到 β[0][0] 的第 k 列，4 次 MISS
                // 若 i=j 还会因为上一步读 A 造成冲突不命中，所以额外造成 3 次冲突不命中。
                B[j][i + k] = a;
                B[j + 1][i + k] = b;
                B[j + 2][i + k] = c;
                B[j + 3][i + k] = d;
                // 将后四个转置后，先复制到 β[0][1] 的第 k 列
                // 可以避免此时去写 β[1][0] 的冲突不命中
                B[j][i + k + 4] = e;
                B[j + 1][i + k + 4] = f;
                B[j + 2][i + k + 4] = g;
                B[j + 3][i + k + 4] = h;
            }
            for(int x=4;x<8;x++){

                a=B[j+x-4][i+4];
                b=B[j+x-4][i+5];
                c=B[j+x-4][i+6];
                d=B[j+x-4][i+7];//1  hit  miss

                 e=A[i+4][j+x-4];
                f=A[i+5][j+x-4];
                g=A[i+6][j+x-4];
                h=A[i+7][j+x-4];//2  miss*4  hit

                
                
               B[j+x-4][i+4]=e;
                B[j+x-4][i+5]=f;
                B[j+x-4][i+6]=g;
                B[j+x-4][i+7]=h;//2 a->b    miss miss


                 B[j+x][i]=a;
                B[j+x][i+1]=b;
                B[j+x][i+2]=c;
                B[j+x][i+3]=d;//1 b->b    miss   miss
                
            }
            
            for(int x=4;x<8;x++){
                a=A[i+x][j+4];
                b=A[i+x][j+5];
                c=A[i+x][j+6];
                d=A[i+x][j+7];
                B[j+4][i+x]=a;
                B[j+5][i+x]=b;
                B[j+6][i+x]=c;
                B[j+7][i+x]=d;
            }

    }
}
    
    
}
    else{      
            int d=16;
            for(int i=0;i<67;i+=d){
                for(int j=0;j<61;j+=d){
                    for(int k=0;k<d&&i+k<67;k++){
                        for(int l=0;l<d&&l+j<61;l++){
                            B[l+j][i+k]=A[i+k][l+j];
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

