#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define WIDTH 18000
#define HEIGHT 14000
#define ITERATIONS 50
#define ARRAYSIZE WIDTH*HEIGHT
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[])
{
    int numtasks, rank, chunksize, leftover, i, j, index;// init[1];//, msgsize;
    unsigned char sendbuf[ARRAYSIZE];
    int histbuf[256], finalbuf[256];
    int histbuf2[256], finalbuf2[256];
    // double tStart, tEnd, tCompute, tAvg, tMin, tMax;
    double scatterStart, scatterEnd, gatherStart, gatherEnd, sendStart, sendEnd, recvStart, recvEnd;
    // double globStart, globEnd, globStart2, globEnd2, t2Min, t2Max, t2Avg, 
    // int testbuf[256];
    MPI_Status status;
    // MPI_Request sendRequest;// recvRequest;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    int gatherbuf[256*numtasks];
    // int initbuf[numtasks];
    // double timebuf[numtasks];
    srand(42);
    gatherStart = scatterEnd = scatterStart = 0.0;
    chunksize = (ARRAYSIZE / numtasks);
    leftover = (ARRAYSIZE % numtasks);
    unsigned char recvbuf[chunksize];

    
    for (i = 0; i < 256; i++) {
        histbuf[i] = 0;
        histbuf2[i] = 0;
    }

    /***************
     * Master task *
     ***************/
    if (rank == MASTER)
    {
        /* Initializing arrays */
        for (i = 0; i < 256; i++) {
            finalbuf[i] = 0;
            finalbuf2[i] = 0;
            sendbuf[i] = (unsigned char)(rand()%256);
        }
        for (; i < ARRAYSIZE; i++) {
            sendbuf[i] = (unsigned char)(rand()%256);
        }
        MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);

        /* 
         * Scatter/Gather 
         */
        MPI_Barrier(MPI_COMM_WORLD);
        scatterStart = MPI_Wtime();
        for (i = 0; i < ITERATIONS; i++)
        {
            MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        }
        scatterEnd = MPI_Wtime();
        for (i = 0; i < leftover; i++) {
            histbuf[sendbuf[i]]++;
        }

        // tStart = MPI_Wtime();
        for (i = 0; i < chunksize; i++) {
            histbuf[recvbuf[i]]++;
        }
        // tEnd = MPI_Wtime();

        gatherStart = MPI_Wtime();
        for (i = 0; i < ITERATIONS; i++)
        {
            MPI_Gather(&histbuf, 256, MPI_INT, &gatherbuf, 256, MPI_INT, MASTER, MPI_COMM_WORLD);
        }
        gatherEnd = MPI_Wtime();
        for (i = 0; i < 256*numtasks; i++)
        {            
            finalbuf[i%256]+=gatherbuf[i];
        }

        /* 
         * Send/Recv
         */
        MPI_Barrier(MPI_COMM_WORLD);
        sendStart = MPI_Wtime();
        for (j = 0; j < ITERATIONS; j++)
        {
            index = leftover + chunksize;
            for (i = 1; i < numtasks; i++)
            {
                MPI_Ssend(&sendbuf[index], chunksize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                index += chunksize;
            }
        }
        sendEnd = MPI_Wtime();
        
        for (i = 0; i < leftover+chunksize; i++) {
            finalbuf2[sendbuf[i]]++;
        }
        recvStart = MPI_Wtime();
        for (j = 0; j < ITERATIONS; j++)
        {
            index = 0;
            for (i = 1; i < numtasks; i++)
            {
                MPI_Recv(&gatherbuf[index], 256, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                index += 256;
            }
        }
        recvEnd = MPI_Wtime();
        for (i = 0; i < 256*(numtasks-1); i++)
        {            
            finalbuf2[i%256]+=gatherbuf[i];
        }
        printf("%04d %f %f ", numtasks, (scatterEnd-scatterStart)/ITERATIONS, (gatherEnd-gatherStart)/ITERATIONS);
        printf("%f %f\n", (sendEnd-sendStart)/ITERATIONS, (recvEnd-recvStart)/ITERATIONS);
    } 
    
    /***************
     * Slave tasks *
     ***************/
    else {
        MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        /* 
         * Scatter/Gather 
         */
        MPI_Barrier(MPI_COMM_WORLD);
        for (i = 0; i < ITERATIONS; i++)
        {
            MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        }
        // tStart = MPI_Wtime();
        for (i = 0; i < chunksize; i++) {
            histbuf[recvbuf[i]]++;
        }
        // tEnd = MPI_Wtime();
        for (i = 0; i < ITERATIONS; i++)
        {
            MPI_Gather(&histbuf, 256, MPI_INT, &gatherbuf, 256, MPI_INT, MASTER, MPI_COMM_WORLD);
        }

        /* 
         * Send/Recv
         */
        MPI_Barrier(MPI_COMM_WORLD);
        for (j = 0; j < ITERATIONS; j++)
        {
            MPI_Recv(&recvbuf, chunksize, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
        }
        for (i = 0; i < chunksize; i++)
        {
            histbuf2[recvbuf[i]]++;
        }
        for (j = 0; j < ITERATIONS; j++)
        {
            MPI_Send(&histbuf2, 256, MPI_INT, MASTER, 1, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
}