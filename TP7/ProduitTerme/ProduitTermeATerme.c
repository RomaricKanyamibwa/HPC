#include <sys/time.h>
#include <stdio.h> 
//#define N 128
#define N 1024
//#define N 16384
//#define N 1048576
//#define N 16777216
//#define N 134217728
#define NB_TIMES 1000000

double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

float A[N];
float B[N];
float C[N];

int main(){
  int i,k;
  double start, stop;
  float res = 0.0; 

  for (i=0;i<N;i++){
    A[i]=1.5;
    B[i]=2.5;
    C[i]=0.0; 
  }

  start = my_gettimeofday();
  
  for (k=0; k<NB_TIMES;k++){

    for (i=0;i<N;i++){
      C[i] = A[i]*B[i]; 
    }
    
  }

  stop = my_gettimeofday(); 
  fprintf(stdout, "C[0]=%f C[1]=%f C[4]=%f C[8]=%f C[N-1]=%f \n", 
	  C[0], C[1], C[4], C[8], C[N-1]); 
  fprintf(stdout, "Temps total de calcul : %g sec\n", stop - start);

  return 0; 
}

