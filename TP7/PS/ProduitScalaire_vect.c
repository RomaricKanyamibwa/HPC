#include <sys/time.h>
#include <stdio.h>
//#define N 1024
#define N 16384
//#define N 1048576
//#define N 16777216
//#define NB_TIMES 1
#define NB_TIMES 1000000
#include <immintrin.h>


double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

float A[N] __attribute__((aligned(32)));
float B[N] __attribute__((aligned(32)));

int main(){
  int i,k,b=0;
  double start, stop;
  float res = 0.0;
  //float Zero[8];
  __m256 v1,v2,v3,z;

  for (i=0;i<N;i++){
    A[i]=1.0;
    B[i]=1.0;
//    if(i<8)
//        Zero[i]=0.0;
  }

  start = my_gettimeofday();
  for (k=0; k<NB_TIMES;k++){
    res = 0.0;

    for (b=0;b<N/8;b++){
        //printf("%d:%d to %d\n",k,b*8,(b+1)*8);
        v1 = _mm256_load_ps(&A[b*8]);
        v2 = _mm256_load_ps(&B[b*8]);
        z  = _mm256_setzero_ps();
        v3 = _mm256_mul_ps(v1,v2);
        for(i=0;i<2;i++)
            v3 = _mm256_hadd_ps(v3,z);//A0+A1 and A0+A1+A2+A3
        //v3 = _mm256_hadd_ps(v3,z);//A0+A1+A2+A3
        //v3 = _mm256_hadd_ps(v3,z);//A0+A1+A2+A3+A4+A5+A6+A7
        res += (float)(v3[0]+v3[4]);
        //printf("%d: %lf %lf %lf %lf %lf %lf %lf %lf \n",k,(float)(v3[0]),(float)(v3[1]),(float)(v3[2]),(float)(v3[3]),(float)(v3[4]),(float)(v3[5]),(float)(v3[6]),(float)(v3[7]));
    }
  }

  stop = my_gettimeofday();
  fprintf(stdout, "res = %f \n", res);
  fprintf(stdout, "Temps total de calcul : %g sec\n", stop - start);

  return 0;
}

