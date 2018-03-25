#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

///////////////////////////////////////////////////////////////////////
/// WARNING: naive algorithm with worst operation count!
int fib(int n) {
  int i, j;
  if (n<2)
    return n;
  else {
    i=fib(n-1);
    j=fib(n-2);
    return i+j;
  }
}


///////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  int i,j;
  double debut=0.0, fin=0.0;
  int n=45; /* default value -> roughly 10 seconds of computation */
  int res=0;

  /* Read 'n' on command line: */
  if (argc == 2){
    n = atoi(argv[1]);
  }

  /* Start timing */
  debut = my_gettimeofday();

  /* Do computation:  */
  res=fib(n);
     
  /* End timing */
  fin = my_gettimeofday();
  fprintf(stdout, " fib(%d)=%d\n", n, res);
  fprintf( stdout, "For n=%d: total computation time (with gettimeofday()) : %g s\n",
	   n, fin - debut);
      
  return 0;
}



