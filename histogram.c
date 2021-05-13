#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define WIDTH 9000
#define HEIGHT 7000
#define ARRAYSIZE WIDTH*HEIGHT
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[])
{
    int numtasks, rank, chunksize, leftover, i;//#, msgsize;
    unsigned char sendbuf[HEIGHT * WIDTH];
    int histbuf[256], finalbuf[256];
    double globStart, globEnd, scatterStart, scatterEnd, gatherStart, gatherEnd, tStart, tEnd, tCompute, tAvg, tMin, tMax;
    // int testbuf[256];
    // MPI_Status status;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    int gatherbuf[256*numtasks];
    double timebuf[numtasks];
    srand(42);
    gatherStart = scatterEnd = scatterStart = globStart = 0.0;
    chunksize = (ARRAYSIZE / numtasks);
    leftover = (ARRAYSIZE % numtasks);
    unsigned char recvbuf[chunksize];
    if (rank == MASTER) {
        globStart = MPI_Wtime();
    }

    for (i = 0; i < 256; i++) {
        histbuf[i] = 0;
    }
    if (rank == MASTER)
    {
        for (i = 0; i < 256; i++) {
            finalbuf[i] = 0;
            sendbuf[i] = (unsigned char)(rand()%256);
            // testbuf[i] = 0;
        }
        for (; i < ARRAYSIZE; i++) {
            sendbuf[i] = (unsigned char)(rand()%256);
        }

        
        // for (int i = 0; i < HEIGHT * WIDTH; i++)
        // {
        //     testbuf[sendbuf[i]]++;
        // }
        
        // printf("\n");
        // for (int i = 0; i < 256; i++)
        // {
        //     printf("%i ", testbuf[i]);
        // }
        // printf("\n");
    }
    if (rank == MASTER) {
        scatterStart = MPI_Wtime();
    }
    MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    tStart = MPI_Wtime();
    if (rank == MASTER) {
        scatterEnd = MPI_Wtime();

        for (i = 0; i < leftover; i++) {
            histbuf[sendbuf[i]]++;
        }
    }
    for (i = 0; i < chunksize; i++) {
        histbuf[recvbuf[i]]++;
    }
    tEnd = MPI_Wtime();
    if (rank == MASTER) {
        gatherStart = MPI_Wtime();
    }
    MPI_Gather(&histbuf, 256, MPI_INT, &gatherbuf, 256, MPI_INT, MASTER, MPI_COMM_WORLD);

    if (rank == MASTER)
    {
        gatherEnd = MPI_Wtime();
        for (i = 0; i < 256*numtasks; i++)
        {            
            // printf("%d + %d = ",finalbuf[i%256], gatherbuf[i]);
            finalbuf[i%256]+=gatherbuf[i];
            // printf("%d\n", finalbuf[i%256]);
        }
        // printf("\n");
        // for (int i = 0; i < 256; i++)
        // {
        //     printf("%i ", finalbuf[i]);
        // }
        // printf("\n");

        globEnd = MPI_Wtime();
        printf("Total: %f\tScatter: %f\tGather: %f\n", globEnd-globStart, scatterEnd-scatterStart, gatherEnd-gatherStart);
    }
    tCompute = tEnd-tStart;
    MPI_Gather(&tCompute, 1, MPI_DOUBLE, timebuf, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
    if (rank == MASTER)
    {
        tAvg = 0;
        tMin = 999999.9;
        tMax = 0.0;

        for (i = 0; i < numtasks; i++) {
            tAvg += timebuf[i];
            if (timebuf[i]>tMax) {
                tMax = timebuf[i];
            }
            if (timebuf[i]<tMin) {
                tMin = timebuf[i];
            }
        }
        tAvg = tAvg/numtasks;
        printf("Compute Min: %f\tAvg: %f\tMax: %f\n", tMin, tAvg, tMax);
    }

    MPI_Finalize();
}