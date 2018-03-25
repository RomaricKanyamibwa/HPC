#include <sys/time.h>
#include <stdio.h> 
//#define N 1024
#define N 16384
//#define N 1048576
//#define N 16777216
//#define NB_TIMES 1
#define NB_TIMES 1000000

double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

float A[N] __attribute__((aligned(32)));
float B[N] __attribute__((aligned(32)));

int main(){
  int i,k;
  double start, stop;
  float res = 0.0;
  __m256 v1,v2,v3;

  for (i=0;i<N;i++){
    A[i]=1.0;
    B[i]=1.0;
  }

  start = my_gettimeofday();

  for (k=0; k<NB_TIMES;k++){
    res = 0.0; 

    for (i=0;i<N/8;i++){
      v1 = _mm256_load_ps(&A[b*8]);
      v2 = _mm256_load_ps(&B[b*8]);
      v3 = _mm256_mul_ps(v1,v2);
      res += A[i]*B[i]; 
    }
  }
    
  stop = my_gettimeofday(); 
  fprintf(stdout, "res = %f \n", res); 
  fprintf(stdout, "Temps total de calcul : %g sec\n", stop - start);

  return 0; 
}

