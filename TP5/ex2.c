#ifdef _OPENMP
#include <omp.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>   /* chronometrage */
#include <sys/time.h>
#define NP 1024



double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

int calcul(int rang)
{
	time_t t;
	srand((unsigned) time(&t));
	//srand(time(NULL));   // should only be called once
	int r = rand();      // returns a pseudo-random integer between 0 and RAND_MAX
	//printf("r:%d\n",r);
	return (r/10*rang)%3==0;
}


int main()
{
	int rang,cpt=0,l_cpt=0,i;
	int index[NP];
	#pragma omp parallel private(rang,l_cpt)
	{
		rang=omp_get_thread_num();
		if(calcul(rang))
		{
			#pragma omp critical
			index[cpt++]=rang;
		}
	}
	printf("cpt:%d\n",cpt);
	for (i = 0; i <cpt; ++i)
	{
		printf("(%d:%d),",i,index[i]);
	}
	printf("\n");

	return 1;
}
