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
   // int i,j,ii,jj;
    int a0=0,a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0,a8=0,a9=0,a10=0,a11=0;


    if(M==32){
        for(int i=0;i<N;i+=8){
            for(int j=0;j<M;j+=8){
                if(i!=j){
                    for(int ii=i;ii<i+8&&ii<N;ii++){
                        for(int jj=j;jj<j+8&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }
                }else{
                    for(int ii=i;ii<i+8&&ii<N;ii++){
                        for(int jj=j;jj<j+8&&jj<M;jj++){
                            if(ii==jj){
                                a0=A[ii][jj];
                            }else{
                                if(jj+1==ii){
                                    B[jj][jj]=a0;
                                }
                                B[jj][ii]=A[ii][jj];
                            }
                        }
                    }
                    B[i+7][j+7]=a0;

                }



            }
        }

        return ;
    }

    if(M==64){
        for(int i=0;i<N;i+=8){
            for(int j=0;j<M;j+=8){
                a0=A[i][j+4];
                a1=A[i][j+5];
                a2=A[i][j+6];
                a3=A[i][j+7];

                a4=A[i+1][j+4];
                a5=A[i+1][j+5];
                a6=A[i+1][j+6];
                a7=A[i+1][j+7];

                a8=A[i+2][j+4];
                a9=A[i+2][j+5];
                a10=A[i+2][j+6];
                a11=A[i+2][j+7];

                if(i!=j){

                    for(int ii=i;ii<i+4&&ii<N;ii++){
                        for(int jj=j;jj<j+4&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }
                    for(int ii=i+4;ii<i+8&&ii<N;ii++){
                        for(int jj=j;jj<j+4&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }
                    for(int ii=i+4;ii<i+8&&ii<N;ii++){
                        for(int jj=j+4;jj<j+8&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }
                    for(int ii=i+3;ii<i+4&&ii<N;ii++){
                        for(int jj=j+4;jj<j+8&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }
                    B[j+4][i]=a0;
                    B[j+5][i]=a1;
                    B[j+6][i]=a2;
                    B[j+7][i]=a3;
                    B[j+4][i+1]=a4;
                    B[j+5][i+1]=a5;
                    B[j+6][i+1]=a6;
                    B[j+7][i+1]=a7;

                    B[j+4][i+2]=a8;
                    B[j+5][i+2]=a9;
                    B[j+6][i+2]=a10;
                    B[j+7][i+2]=a11;


                }else{
                    int a;
                    for(int ii=i;ii<i+4&&ii<N;ii++){
                        for(int jj=j;jj<j+4&&jj<M;jj++){
                            if(ii==jj){
                                a=A[ii][jj];
                            }else{
                                if(jj+1==ii){
                                    B[jj][jj]=a;
                                }
                                B[jj][ii]=A[ii][jj];
                            }
                        }
                    }
                    B[i+3][j+3]=a;

                    for(int ii=i+4;ii<i+8&&ii<N;ii++){
                        for(int jj=j;jj<j+4&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }


                    for(int ii=i+4;ii<i+8&&ii<N;ii++){
                        for(int jj=j+4;jj<j+8&&jj<M;jj++){
                            if(ii==jj){
                                a=A[ii][jj];
                            }else{
                                if(jj+1==ii){
                                    B[jj][jj]=a;
                                }
                                B[jj][ii]=A[ii][jj];
                            }
                        }
                    }
                    B[i+7][j+7]=a;


                    for(int ii=i+3;ii<i+4&&ii<N;ii++){
                        for(int jj=j+4;jj<j+8&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }
                    B[j+4][i]=a0;
                    B[j+5][i]=a1;
                    B[j+6][i]=a2;
                    B[j+7][i]=a3;
                    B[j+4][i+1]=a4;
                    B[j+5][i+1]=a5;
                    B[j+6][i+1]=a6;
                    B[j+7][i+1]=a7;

                    B[j+4][i+2]=a8;
                    B[j+5][i+2]=a9;
                    B[j+6][i+2]=a10;
                    B[j+7][i+2]=a11;

                }

            }
        }

        return ;
    }

    if(M==61){
        for(int i=0;i<N;i+=8){
            for(int j=0;j<M;j+=8){
                for(int ii=i;ii<i+8&&ii<N;ii++){
                    for(int jj=j;jj<j+8&&jj<M;jj++){
                        B[jj][ii]=A[ii][jj];
                    }

                }
            }
        }
        return ;

        /*for(int i=0;i<N;i+=8){
            for(int j=0;j<M;j+=8){
                if(i!=j){
                    for(int ii=i;ii<i+8&&ii<N;ii++){
                        for(int jj=j;jj<j+8&&jj<M;jj++){
                            B[jj][ii]=A[ii][jj];
                        }
                    }
                }else{
                    for(int ii=i;ii<i+8&&ii<N;ii++){
                        for(int jj=j;jj<j+8&&jj<M;jj++){
                            if(ii==jj){
                                a0=A[ii][jj];
                            }else{
                                if(jj+1==ii){
                                    B[jj][jj]=a0;
                                }
                                B[jj][ii]=A[ii][jj];
                            }
                        }
                    }
                    B[i+7][j+7]=a0;

                }



            }
        }

        return ;*/
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
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            B[j][i]  = A[i][j];

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

