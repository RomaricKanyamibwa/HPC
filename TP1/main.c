#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <unistd.h>

#define SIZE_H_N 50

int main(int argc,char* argv[])
{
    int my_rank,p,source,dest,tag=0;
    char message[100];
    char hostname[SIZE_H_N];
    MPI_Status status;

    gethostname(hostname,SIZE_H_N);

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    // printf("Start Program from %s process #%d\n",hostname,my_rank);
    if(my_rank==0)
    {
        // printf("Send message from %s process #%d\n",hostname,my_rank);
        sprintf(message,"Hello process #%d depuis %s!",my_rank,hostname);
        dest=(my_rank<p-1)?my_rank+1:0;
    printf("Process #%d dest is #%d\n",my_rank,dest);
        MPI_Ssend(message,strlen(message)+1,MPI_CHAR,dest,tag,MPI_COMM_WORLD);
        // printf("END Send message from %s process #%d\n",hostname,my_rank);
    source=(my_rank==0)?p-1:my_rank-1;
    MPI_Recv(message,SIZE_H_N,MPI_CHAR,source,tag,MPI_COMM_WORLD,&status);

    }
    else
    {
        // printf("Received message (host:%s,#%d)\n",hostname,my_rank);
        //for (source = 1; source < p; source++)
        //{
                /* code */
    source=(my_rank==0)?p-1:my_rank-1;
    printf("Process #%d source is #%d\n",my_rank,source);
        MPI_Recv(message,SIZE_H_N,MPI_CHAR,source,tag,MPI_COMM_WORLD,&status);
        printf("The host %s,has received  from the process #%d the message:%s \n",hostname,my_rank,message);
    sprintf(message,"Hello process #%d depuis %s!",my_rank,hostname);
    dest=(my_rank<p-1)?my_rank+1:0;
    MPI_Ssend(message,strlen(message)+1,MPI_CHAR,dest,tag,MPI_COMM_WORLD);

        //}
        // printf("END received message (host:%s,#%d)\n",hostname,my_rank);

    }

    MPI_Finalize();
    return 1;

}
