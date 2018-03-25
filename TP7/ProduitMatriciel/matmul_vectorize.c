/*
 * Université Pierre et Marie Curie
 *
 * Programme de multiplication de matrices carrees.
 */

#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include <malloc.h>

#include <sys/time.h>

double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}


#define REAL_T float
#define NB_TIMES 10

/*** Matmul: ***/
/* C += A x B
 * square matrices of order 'n'
 */
void matmul(int n, REAL_T *A, REAL_T *B, REAL_T *C){
  int i,j,k,l;
  __m256 v1,v2,v3,z;

  for (i=0; i<n; i++){
    for (j=0; j<n; j++){
      for (k=0; k<n/8; k++){
        v1 = _mm256_load_ps(&A[i*n+k*8]);
        v2 = _mm256_load_ps(&B[j*n+k*8]);
        z  = _mm256_setzero_ps();
        v3 = _mm256_mul_ps(v1,v2);
        for(l=0;l<2;l++)
        v3 = _mm256_hadd_ps(v3,z);//A0+A1 and A0+A1+A2+A3
        C[i*n+j] += (REAL_T)(v3[0]+v3[4]);
      } /* for k */
    } /* for j */
  } /* for i */

}

REAL_T* transpose(int n,REAL_T* B)
{
    int i,j;
    REAL_T* C;
    if ((C = (REAL_T *) aligned_alloc(32,n*n*sizeof(REAL_T))) == NULL){
        fprintf(stderr, "Error while allocating C.\n");
    }
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++){
          *(C+j*n+i) = *(B+i*n+j);
        }
    return C;

}

int main(int argc, char **argv)
{
  int i,j;
  double debut=0.0, fin=0.0;
  REAL_T *A, *B, *C;
  int n=8; /* default value */
  int nb=0;

  /* Read 'n' on command line: */
  if (argc == 2){
    n = atoi(argv[1]);
  }
  if(n%8!=0)
  {
      fprintf(stderr,"Error:N=%d is not a multiple of 8, can not perform vectorisation\n",n);
      exit(-1);
  }

  /* Allocate the matrices: */
  if ((A = (REAL_T *) aligned_alloc(32,n*n*sizeof(REAL_T))) == NULL){
    fprintf(stderr, "Error while allocating A.\n");
  }
  if ((B = (REAL_T *) aligned_alloc(32,n*n*sizeof(REAL_T))) == NULL){
    fprintf(stderr, "Error while allocating B.\n");
  }
  if ((C = (REAL_T *) aligned_alloc(32,n*n*sizeof(REAL_T))) == NULL){
    fprintf(stderr, "Error while allocating C.\n");
  }

  /* Initialize the matrices */
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++){
      *(A+i*n+j) = 1 / ((REAL_T) (i+j+1));
      *(B+i*n+j) = 1.0+j;
      *(C+i*n+j) = 1.0;
    }
//    for(i=0; i<n ; i++){
//    for(j=0; j<n ; j++)
//      printf("%+e  ", B[i*n+j]);
//    printf("\n");
//  }
    B=transpose(n,B);
//    printf("Transposed matrix\n");
//    for(i=0; i<n ; i++){
//    for(j=0; j<n ; j++)
//      printf("%+e  ", B[i*n+j]);
//    printf("\n");
//  }
  /* Start timing */
  debut = my_gettimeofday();
  for (nb=0; nb<NB_TIMES; nb++){
    /* Do matrix-product C=A*B+C */
    matmul(n, A, B, C);
    /* End timing */
  }
  fin = my_gettimeofday();

  fprintf( stdout,"Vectorized result.\n");
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
