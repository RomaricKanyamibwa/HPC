#ifdef _OPENMP
#include <omp.h>
#endif

// C program to multiply two square matrices.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   /* chronometrage */
#include <sys/time.h>
#define N 10


double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

 
// This function multiplies mat1[][] and mat2[][],
// and stores the result in res[][]
void multiply(int** mat1, int** mat2, int** res)
{
    int i, j, k;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            res[i][j] = 0;
            for (k = 0; k < N; k++)
                res[i][j] += mat1[i][k]*mat2[k][j];
        }
    }
}

int **random_matrix(int m,int n)
{
  int i,j,k;
  int **x;
  time_t t;
  srand((unsigned) time(&t));
  x = (int **) malloc(m*sizeof(int));
  for(i=0;i<m;i++)
    x[i] = (int *) malloc(n*sizeof(int));
  for(i=0;i<m;i++)
  {
    for(j=0;j<n;j++)
    {
      x[i][j]=rand()%100;
      printf("%d\t",x[i][j]);
    }
    printf("\n");
  }
  return x;
}

int main()
{
    int** mat1;
    mat1=random_matrix(N,N);

    printf("\n");
 
    int** mat2;
    mat2=random_matrix(N,N);
    int i, j;
    int** res; // To store result
    
    res = (int **) malloc(N*sizeof(int));
    for(i=0;i<N;i++)
      res[i] = (int *) malloc(N*sizeof(int));
    
    multiply(mat1, mat2, res);
 
    printf("Result matrix is \n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
           printf("%d ", res[i][j]);
        printf("\n");
    }
 
    return 0;
}