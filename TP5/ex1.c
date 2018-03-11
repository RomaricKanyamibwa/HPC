#ifdef _OPENMP
#include <omp.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>   /* chronometrage */
#include <sys/time.h>

#define N 1000000000


double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}


double pi_0()
{
	double pi=0,e;
	int i;
	for (i = 0; i < N; ++i)
	{
		e=(double)i/N;
		pi+=4.0/(1.0+e*e);
	}
	return pi/N;
}

double pi_1()
{
	double pi=0,e;
	int i;
	#pragma omp parallel for reduction(+:pi)
	for (i = 0; i < N; ++i)
	{
		e=(double)i/N;
		pi+=4.0/(1.0+e*e);
	}
	return pi/N;
}

double pi_2()
{
	double pi=0,e,pi_loc=0;
	int i;
	#pragma omp parallel private(pi_loc,e)
	{
		#pragma omp for
			for (i = 0; i < N; ++i)
			{
				e=(double)i/N;
				pi_loc+=4.0/(1.0+e*e);
			}
		#pragma omp critical
			pi+=pi_loc;

	}

	return pi/N;
}

double pi_3()
{
	double pi=0,e;
	int i;
	#pragma omp parallel for private(e)
	for (i = 0; i < N; ++i)
	{
		e=(double)i/N;
		#pragma omp atomic
		pi+=4.0/(1.0+e*e);
	}
	return pi/N;
}

int main()
{
	/* Variables liees au chronometrage */
  	double debut, fin;

  	debut = my_gettimeofday();
	double pi = pi_0();
	fin = my_gettimeofday();
	printf("Res=%lf\n",pi);
    printf("#Temps total de calcul pi : %g seconde(s) \n",fin - debut);

  	debut = my_gettimeofday();
	pi = pi_1();
	fin = my_gettimeofday();
	printf("Res=%lf\n",pi);
    printf("#Temps total de calcul pi_1: %g seconde(s) \n",fin - debut);

    debut = my_gettimeofday();
	pi = pi_2();
	fin = my_gettimeofday();
	printf("Res=%lf\n",pi);
    printf("#Temps total de calcul pi_2: %g seconde(s) \n",fin - debut);

    debut = my_gettimeofday();
	pi = pi_3();
	fin = my_gettimeofday();
	printf("Res=%lf\n",pi);
    printf("#Temps total de calcul pi_3: %g seconde(s) \n",fin - debut);

	return 1;
}
