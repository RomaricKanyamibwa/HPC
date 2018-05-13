/*
 * Sorbonne Université
 *
 * Programme de multiplication de matrices carrees.
 */
#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>
#include <cblas.h>

double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}


#define REAL_T double
#define NB_TIMES 10

/*** Matmul: ***/
/* C += A x B
 * square matrices of order 'n'
 */
void matmul(int n, REAL_T *A, REAL_T *B, REAL_T *C){
  int i,j,k;

  #pragma omp parallel for private(j,k)
  for (i=0; i<n; i++){
    for (j=0; j<n; j++){
      for (k=0; k<n; k++){
	C[i*n+j] +=  A[i*n+k] *  B[k*n+j];
      } /* for k */
    } /* for j */
  } /* for i */

}

void mm(int crow,int ccol,
        int arow,int acol,
        int brow,int bcol,
        int n,
        int stride,
        REAL_T *A, REAL_T *B, REAL_T *C){
  int i,j,k;
unsigned short seuil=4096;
if(n*n<seuil)
{
    //C[ccol+crow*stride]+=A[acol+arow*stride]*B[bcol+brow*stride];
    #pragma omp parallel for private(j,k)
      for (i=0; i<n; i++){
        for (j=0; j<n; j++){
          for (k=0; k<n; k++){
            C[(crow+i)*stride+j+ccol] +=  A[(i+arow)*stride+k+acol] *  B[(k+brow)*stride+j+bcol];
          } // for k
        } // for j
      } // for i
}else
{
    int i,j,k=0;
//    for(i=0;i<2;i++)
//    {
//        for(j=0;j<2;j++)
//        {
//            for(k=0;k<2;k++)
//            {
//                mm(crow+i*n/2,ccol+j*n/2,
//                arow+i*n/2,acol+k*n/2,
//                brow+k*n/2,bcol+j*n/2,
//                n/2,stride,A,B,C);
//            }
//        }
//    }

    for(i=0;i<2;i++)
    {
        for(j=0;j<2;j++)
        {
        #pragma omp task
            mm(crow+i*n/2,ccol+j*n/2,
            arow+i*n/2,acol+k*n/2,
            brow+k*n/2,bcol+j*n/2,
            n/2,stride,A,B,C);
        }
    }

    #pragma omp taskwait
    k=1;
    for(i=0;i<2;i++)
    {
        for(j=0;j<2;j++)
        {
        #pragma omp task
            mm(crow+i*n/2,ccol+j*n/2,
            arow+i*n/2,acol+k*n/2,
            brow+k*n/2,bcol+j*n/2,
            n/2,stride,A,B,C);
        }
    }
}

}


void matmul_blas(int n,REAL_T *A, REAL_T *B, REAL_T *C){

int M;     /* Number of row of matrix op(A) */
int N;     /* Number of columns of matrix op(B) */
int K;     /* Number of columns of matrix op(A) and rows ob(B) */
int lda;   /* On entry, LDA specifies the first dimension of A as declared
              in the calling (sub) program. When  TRANSA = 'N' or 'n' then
              LDA must be at least  max( 1, m ), otherwise  LDA must be at
              least  max( 1, k ).  */
int ldb;   /* On entry, LDB specifies the first dimension of B as declared
              in the calling (sub) program. When  TRANSB = 'N' or 'n' then
              LDB must be at least  max( 1, k ), otherwise  LDB must be at
              least  max( 1, n ). */
int ldc;
double alpha, beta;

M=N=K=n;
lda=ldb=ldc=n;
alpha=1.0;
beta=0.0;
cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
M,N,K,alpha,A,lda,B,ldb,beta,C,ldc);

}

int main(int argc, char **argv)
{
  int i,j;
  double debut=0.0, fin=0.0;
  REAL_T *A, *B, *C;
  int n=2; /* default value */
  int nb=0;

  /* Read 'n' on command line: */
  if (argc == 2){
    n = atoi(argv[1]);
  }

  /* Allocate the matrices: */
  if ((A = (REAL_T *) malloc(n*n*sizeof(REAL_T))) == NULL){
    fprintf(stderr, "Error while allocating A.\n");
  }
  if ((B = (REAL_T *) malloc(n*n*sizeof(REAL_T))) == NULL){
    fprintf(stderr, "Error while allocating B.\n");
  }
  if ((C = (REAL_T *) malloc(n*n*sizeof(REAL_T))) == NULL){
    fprintf(stderr, "Error while allocating C.\n");
  }

  /* Initialize the matrices */
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++){
      *(A+i*n+j) = 1 / ((REAL_T) (i+j+1));
      *(B+i*n+j) = 1.0;
      *(C+i*n+j) = 1.0;
    }

  /* Start timing */
  debut = my_gettimeofday();
  for (nb=0; nb<NB_TIMES; nb++){
    /* Do matrix-product C=A*B+C */
    //matmul(n, A, B, C);
    //mm(0,0,0,0,0,0,n,n,A,B,C);
    matmul_blas(n,A,B,C);
    /* End timing */
  }
  fin = my_gettimeofday();

  fprintf( stdout, "For n=%d: total computation time (with gettimeofday()) : %g s\n",
	   n, (fin - debut)/NB_TIMES);
  fprintf( stdout, "For n=%d: performance = %g Gflop/s \n",
	   n, (((double) 2)*n*n*n / ((fin - debut)/NB_TIMES) )/ ((double) 1e9) ); /* 2n^3 flops */

  /* Print 2x2 top-left square of C : */
  for(i=0; i<2 ; i++){
    for(j=0; j<2 ; j++)
      printf("%+e  ", C[i*n+j]);
    printf("\n");
  }
  printf("\n");
  /* Print 2x2 bottom-right square of C : */
  for(i=n-2; i<n ; i++){
    for(j=n-2; j<n ; j++)
      printf("%+e  ", C[i*n+j]);
    printf("\n");
  }

  /* Free the matrices: */
  free(A);
  free(B);
  free(C);

  return 0;
}
