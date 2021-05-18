#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define ITERATIONS 20
#define SIZE 10000
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[])
{
    int numtasks, rank, chunksize, leftover, i;
    unsigned char sendbuf[SIZE];
    double times[ITERATIONS];

    double tStart, tEnd;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    srand(42);
    tStart = tEnd = 0.0;
    chunksize = (SIZE / numtasks);
    leftover = (SIZE % numtasks);
    unsigned char recvbuf[chunksize];
    for (i = 0; i < ITERATIONS; i++)
    {
        times[i]=0.0;
    }
    if (rank == MASTER)
    {
        for (i = 0; i < SIZE; i++) {
            sendbuf[i] = (unsigned char)(rand()%256);            
        }
        for (i = 0; i < ITERATIONS; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            tStart = MPI_Wtime();
            MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
            tEnd = MPI_Wtime();
            times[i] = tEnd - tStart;
        }
        for (i = 0; i < ITERATIONS; i++)
        {
            printf("%02d %f\n", i+1, times[i]);
        }
        printf("\n");
    } else {
        for (i = 0; i < ITERATIONS; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        }
    }
    // MPI_Barrier(MPI_COMM_WORLD);
    // if (rank == MASTER) {
    // }


    // if (rank == MASTER)
    // {
    //     for (i = 0; i < SIZE; i++) {
    //         sendbuf[i] = (unsigned char)(rand()%256);            
    //     }
    // }
    // // MPI_Barrier(MPI_COMM_WORLD);
    // for (i = 0; i < ITERATIONS; i++)
    // {
    //     if (rank == MASTER) {
    //         tStart = MPI_Wtime();
    //     }
    //     MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    //     if (rank == MASTER) {
    //         tEnd = MPI_Wtime();
    //         times[i] = tEnd - tStart;
    //     }
    // }
    // if (rank == MASTER) {
    //     for (i = 0; i < ITERATIONS; i++)
    //     {
    //         printf("%02d %f\n", i+1, times[i]);
    //     }
    //     printf("\n");
    // }
    MPI_Finalize();
}