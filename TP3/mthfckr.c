/*
 * Université Sorbonne
 * Calcul de l'ensemble de Mandelbrot, Version parallele
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>   /* chronometrage */
#include <string.h>     /* pour memset */
#include <math.h>
#include <sys/time.h>

#include "rasterfile.h"
#include <mpi.h>
#include <unistd.h>

#define SIZE_H_N 50


 char info[] = "\
Usage:\n\
      mandel dimx dimy xmin ymin xmax ymax prof nblines\n\
\n\
      dimx,dimy : dimensions de l'image a generer\n\
      xmin,ymin,xmax,ymax : domaine a calculer dans le plan complexe\n\
      prof : nombre maximale d'iteration\n\
\n\
Quelques exemples d'execution\n\
      mandel 800 800 0.35 0.355 0.353 0.358 200 2\n\
      mandel 800 800 -0.736 -0.184 -0.735 -0.183 500 2\n\
      mandel 800 800 -0.736 -0.184 -0.735 -0.183 300 2\n\
      mandel 800 800 -1.48478 0.00006 -1.48440 0.00044 100 2\n\
      mandel 800 800 -1.5 -0.1 -1.3 0.1 10000 2\n\
";



double my_gettimeofday(){
  struct timeval tmp_time;
  gettimeofday(&tmp_time, NULL);
  return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}




/**
 * Convertion entier (4 octets) LINUX en un entier SUN
 * @param i entier à convertir
 * @return entier converti
 */

int swap(int i) {
  int init = i;
  int conv;
  unsigned char *o, *d;

  o = ( (unsigned char *) &init) + 3;
  d = (unsigned char *) &conv;

  *d++ = *o--;
  *d++ = *o--;
  *d++ = *o--;
  *d++ = *o--;

  return conv;
}


/***
 * Par Francois-Xavier MOREL (M2 SAR, oct2009):
 */

unsigned char power_composante(int i, int p) {
  unsigned char o;
  double iD=(double) i;

  iD/=255.0;
  iD=pow(iD,p);
  iD*=255;
  o=(unsigned char) iD;
  return o;
}

unsigned char cos_composante(int i, double freq) {
  unsigned char o;
  double iD=(double) i;
  iD=cos(iD/255.0*2*M_PI*freq);
  iD+=1;
  iD*=128;

  o=(unsigned char) iD;
  return o;
}

/***
 * Choix du coloriage : definir une (et une seule) des constantes
 * ci-dessous :
 */
#define ORIGINAL_COLOR
//#define COS_COLOR

#ifdef ORIGINAL_COLOR
#define COMPOSANTE_ROUGE(i)    ((i)/2)
#define COMPOSANTE_VERT(i)     ((i)%190)
#define COMPOSANTE_BLEU(i)     (((i)%120) * 2)
#endif /* #ifdef ORIGINAL_COLOR */
#ifdef COS_COLOR
#define COMPOSANTE_ROUGE(i)    cos_composante(i,13.0)
#define COMPOSANTE_VERT(i)     cos_composante(i,5.0)
#define COMPOSANTE_BLEU(i)     cos_composante(i+10,7.0)
#endif /* #ifdef COS_COLOR */


/**
 *  Sauvegarde le tableau de données au format rasterfile
 *  8 bits avec une palette de 256 niveaux de gris du blanc (valeur 0)
 *  vers le noir (255)
 *    @param nom Nom de l'image
 *    @param largeur largeur de l'image
 *    @param hauteur hauteur de l'image
 *    @param p pointeur vers tampon contenant l'image
 */

void sauver_rasterfile( char *nom, int largeur, int hauteur, unsigned char *p) {
  FILE *fd;
  struct rasterfile file;
  int i;
  unsigned char o;

  if ( (fd=fopen(nom, "w")) == NULL ) {
    printf("erreur dans la creation du fichier %s \n",nom);
    exit(1);
  }

  file.ras_magic  = swap(RAS_MAGIC);
  file.ras_width  = swap(largeur);    /* largeur en pixels de l'image */
  file.ras_height = swap(hauteur);         /* hauteur en pixels de l'image */
  file.ras_depth  = swap(8);              /* profondeur de chaque pixel (1, 8 ou 24 )   */
  file.ras_length = swap(largeur*hauteur); /* taille de l'image en nb de bytes      */
  file.ras_type    = swap(RT_STANDARD);   /* type de fichier */
  file.ras_maptype = swap(RMT_EQUAL_RGB);
  file.ras_maplength = swap(256*3);

  fwrite(&file, sizeof(struct rasterfile), 1, fd);

  /* Palette de couleurs : composante rouge */
  i = 256;
  while( i--) {
    o = COMPOSANTE_ROUGE(i);
    fwrite( &o, sizeof(unsigned char), 1, fd);
  }

  /* Palette de couleurs : composante verte */
  i = 256;
  while( i--) {
    o = COMPOSANTE_VERT(i);
    fwrite( &o, sizeof(unsigned char), 1, fd);
  }

  /* Palette de couleurs : composante bleu */
  i = 256;
  while( i--) {
    o = COMPOSANTE_BLEU(i);
    fwrite( &o, sizeof(unsigned char), 1, fd);
  }

  // pour verifier l'ordre des lignes dans l'image :
  //fwrite( p, largeur*hauteur/3, sizeof(unsigned char), fd);

  // pour voir la couleur du '0' :
  // memset (p, 0, largeur*hauteur);

  fwrite( p, largeur*hauteur, sizeof(unsigned char), fd);
  fclose( fd);
}

/**
 * Étant donnée les coordonnées d'un point \f$c=a+ib\f$ dans le plan
 * complexe, la fonction retourne la couleur correspondante estimant
 * à quelle distance de l'ensemble de mandelbrot le point est.
 * Soit la suite complexe défini par:
 * \f[
 * \left\{\begin{array}{l}
 * z_0 = 0 \\
 * z_{n+1} = z_n^2 + c
 * \end{array}\right.
 * \f]
 * le nombre d'itérations que la suite met pour diverger est le
 * nombre \f$ n \f$ pour lequel \f$ |z_n| > 2 \f$.
 * Ce nombre est ramené à une valeur entre 0 et 255 correspond ainsi a
 * une couleur dans la palette des couleurs.
 */

unsigned char xy2color(double a, double b, int prof) {
  double x, y, temp, x2, y2;
  int i;

  x = y = 0.;
  for( i=0; i<prof; i++) {
    /* garder la valeur précédente de x qui va etre ecrase */
    temp = x;
    /* nouvelles valeurs de x et y */
    x2 = x*x;
    y2 = y*y;
    x = x2 - y2 + a;
    y = 2*temp*y + b;
    if( x2 + y2 > 4.0) break;
  }
  return (i==prof)?255:(int)((i%255));
}

/* Domaine de calcul dans le plan complexe */
double xmin, ymin;
double xmin_prime, ymin_prime;
double xmax, ymax;
/* Dimension de l'image */
int w,h,hprime;
/* Pas d'incrementation */
double xinc, yinc;
/* Profondeur d'iteration */
int prof;

int nblocs/*nombre de blocs que on va calculer*/,nlines/*nombre de lignes (par defaut 1)*/,rank_master,rank;

#define TAG_MESSAGE

#ifdef TAG_MESSAGE
#define TAG_END 0
#define TAG_REQ 1
#define TAG_RESP 2
#endif /* #ifdef TAG_MESSAGE */

unsigned char* mandel_seq(int numbloc/*,unsigned char* pointeur*/)
{
  unsigned char *ima,*pima;
  ima=pima = (unsigned char *)malloc( w*nlines*sizeof(unsigned char));
  unsigned int j,i;
  double x, y;
  ymin_prime = ymin+nlines*numbloc*xinc;
  y = ymin_prime;
  for (i = 0; i < nlines; i++) {
    x = xmin_prime;
    for (j = 0; j < w; j++) {
      *pima++ = xy2color( x, y, prof);
      x += xinc;
    }
    y += yinc;
  }
  //printf("SEQQQ\n");
  if(numbloc % 200==0)
  printf("SEQ#%d=> %d\n",numbloc,ima[400] );
  //pointeur=ima;
  return ima;
}

int worker(int numbloc)
{
  unsigned char *pima;
  pima = (unsigned char *)malloc( w*nlines*sizeof(unsigned char));
  MPI_Status status;
  unsigned int blockid;
  while(1)
  {
    MPI_Recv(&blockid,sizeof(int),MPI_INT,rank_master,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
    if(status.MPI_TAG==TAG_END)
    {
      printf("ENDDDD of worker #%d.\n",rank);
      break;
    }
    else if(status.MPI_TAG==TAG_REQ)
    {

      pima=mandel_seq(blockid);
      if(blockid % 200==0)
      printf("Worker_SEQ#%d: %d\n",blockid,pima[400] );
      //printf("----------------P#%d:Receiving blockid=%d-------------------------\n",rank,blockid);
      MPI_Send(&blockid,sizeof(unsigned int),MPI_UNSIGNED,rank_master,TAG_REQ,MPI_COMM_WORLD);
      //printf("process #%d is Sending blockid #%d.\n",rank,blockid);
      MPI_Send(pima,w*nlines,MPI_UNSIGNED_CHAR,rank_master,TAG_RESP,MPI_COMM_WORLD);
      //printf("process #%d is Sending data.\n",rank);
    }
    else
    {
      printf("-------------------UNKNOWN TAG-----------------------------------\n");
    }
  }
  return 0;
}

int master(int NP,unsigned char *pima)
{
  unsigned int blockid=0,blockidresp,i/*n*/;
  MPI_Status status;

  if(NP<=1)
  {
      fprintf(stderr,"ERROR:You must execute with at least 2 processes(mpirun -n 2)\n");
      return 1;
  }

  for(i=1;i<NP;i++)
  {
    MPI_Send(&blockid,sizeof(unsigned int),MPI_INT,i,TAG_REQ,MPI_COMM_WORLD);
    //printf("Sending req to #%d\n",i);
    blockid++;
  }
  //MPI_Send(ima,w*hprime,MPI_UNSIGNED_CHAR,dest,tag,MPI_COMM_WORLD);
  // blockid=0;
  //printf("RESPONSE\n");
  printf("============================ MESSAGE non Reçu:%d ============================\n",nblocs-(blockid-NP)-1);
  while(blockid<nblocs || nblocs-(blockid-NP)-1!=0)
  {
    MPI_Recv(&blockidresp,sizeof(unsigned int),MPI_UNSIGNED,MPI_ANY_SOURCE,TAG_REQ,MPI_COMM_WORLD,&status);
    //printf("_________________Master is Receiving blockid=%d from process #%d_________________\n",blockidresp,status.MPI_SOURCE);
    MPI_Recv(pima+w*nlines*blockidresp,w*nlines,MPI_UNSIGNED_CHAR,status.MPI_SOURCE,TAG_RESP,MPI_COMM_WORLD,&status);
    //printf("Master is Receiving blocki=%d data from process #%d\n",blockidresp,status.MPI_SOURCE);
    //memcpy(pima+w*nlines*blockidresp,pima_loc,w*nlines);
    //printf("COPYY!!!!\n");
    if(blockidresp >=790 ||blockidresp % 200==0 )
        printf("MASTER_SEQ#%d: %d\n",blockidresp,pima[400+w*nlines*blockidresp] );
    MPI_Send(&blockid,sizeof(unsigned int),MPI_INT,status.MPI_SOURCE,TAG_REQ,MPI_COMM_WORLD);
    blockid++;
    printf("============================ MESSAGE non Reçu:%d ============================\n",nblocs-(blockid-NP)-1);
    //blockid+=blockidresp;
    //printf("%d============================ MESSAGE Reçu:%d ============================\n",blockidresp,nbm);
  }

//  printf("============================ MESSAGE non Reçu:%d ============================\n",nblocs-(blockid-NP));
//  for(n=1;n<NP;n++)
//  {
//    MPI_Irecv(&blockidresp,sizeof(unsigned int),MPI_UNSIGNED,n,TAG_REQ,MPI_COMM_WORLD,&status);
//    MPI_Irecv(pima+w*nlines*blockidresp,w*nlines,MPI_UNSIGNED_CHAR,status.MPI_SOURCE,TAG_RESP,MPI_COMM_WORLD,&status);
//  }
//  n=0;
//  while(n<nblocs-(blockid-NP))
//  {
//    MPI_Recv(&blockidresp,sizeof(unsigned int),MPI_UNSIGNED,MPI_ANY_SOURCE,TAG_REQ,MPI_COMM_WORLD,&status);
//    MPI_Recv(pima+w*nlines*blockidresp,w*nlines,MPI_UNSIGNED_CHAR,status.MPI_SOURCE,TAG_RESP,MPI_COMM_WORLD,&status);
//    //if(blockidresp % 200==0 )
//    printf("MASTER_SEQ#%d: %d\n",blockidresp,pima[400+w*nlines*blockidresp] );
//    printf("Message#%d:\n",n++);
//  }
//  printf("============================ MESSAGE Reçu:%d ============================\n",n);
  for(i=1;i<NP;i++)
  {
    MPI_Send(&blockid,sizeof(unsigned int),MPI_INT,i,TAG_END,MPI_COMM_WORLD);
    //printf("Sending req to #%d\n",i);
  }
  return 0;
}

/*
 * Partie principale: en chaque point de la grille, appliquer xy2color
 */

int main(int argc,char* argv[])
{
    int my_rank,p/*Nombre de processeur,source,dest,tag=0*/;
    //char hostname[SIZE_H_N];
    //MPI_Status status;
    /* Image resultat */
    unsigned char /**pima_loc,*/ *pima;
    /* Variables intermediaires */
    //int  i, j;
    //double x, y;

    /* Chronometrage */
    double debut, fin;


    //gethostname(hostname,SIZE_H_N);

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

    if( argc == 1 && my_rank==0)
    {
        fprintf( stderr, "%s\n", info);
        //return 0;
    }

    /* Valeurs par defaut de la fractale */
    xmin = -2; ymin = -2;
    xmax =  2; ymax =  2;
    w = h = 800;
    prof = 10000;
    nlines=1;


    /* Recuperation des parametres */
    if( argc > 1) w    = atoi(argv[1]);
    if( argc > 2) h    = atoi(argv[2]);
    if( argc > 3) xmin = atof(argv[3]);
    if( argc > 4) ymin = atof(argv[4]);
    if( argc > 5) xmax = atof(argv[5]);
    if( argc > 6) ymax = atof(argv[6]);
    if( argc > 7) prof = atoi(argv[7]);
    if( argc > 8) nlines = atoi(argv[8]);

    nblocs=h/nlines;
    rank_master=0;
    rank=my_rank;
    hprime=h/nlines;

    /* Calcul des pas d'incrementation */
    xinc = (xmax - xmin) / (w-1);
    yinc = (ymax - ymin) / (h-1);
    xmin_prime = xmin;
    //ymin_prime = ymin+my_rank*hprime*xinc;

    /* Allocation memoire du tableau resultat */
    pima = (unsigned char *)malloc( w*h*sizeof(unsigned char));
    if( pima == NULL) {
      fprintf( stderr, "Erreur allocation mémoire du tableau \n");
      return 0;
    }
    if(my_rank==rank_master)
    {
        /* affichage parametres pour verificatrion */
        fprintf( stderr, "Domaine: {[%lg,%lg]x[%lg,%lg]}\n", xmin, ymin, xmax, ymax);
        fprintf( stderr, "Increment : %lg %lg\n", xinc, yinc);
        fprintf( stderr, "Prof: %d\n",  prof);
        fprintf( stderr, "Dim image: %dx%d\n", w, h);
        fprintf( stderr,"Nombre proc %d\n",p);
        fprintf( stderr,"Lines %d\n\n",nlines);

        debut=MPI_Wtime();
        master(p,pima);
        fin=MPI_Wtime();

        printf("Mandelbrot SET HPC\n Begin Master;\n");
    	// printf("Send message from %s process #%d\n",hostname,my_rank);
    	int i;
    	for(i=0;i<h;i++)
            if(i% 200==0)
                printf("MAIN_SEQ#%d: %d\n",i,pima[400+w*nlines*i] );
            printf("END of Mandelbrot SET HPC #%d\n",my_rank);
        /* Sauvegarde de la grille dans le fichier resultat "mandel.ras" */
        sauver_rasterfile( "mandel_paral_worker.ras", w, h, pima);
        fprintf( stderr, "[Temps total de calcul:%g sec]\n",fin - debut);
    }
    else
    {
        worker(0);
    }
    free(pima);
    //free(pima_loc);
    MPI_Finalize();
    return 0;

}
