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
    double scatterStart, scatterEnd, gatherStart, gatherEnd, sendStart, sendEnd, recvStart, recvEnd;
    // double globStart, globEnd, globStart2, globEnd2, t2Min, t2Max, t2Avg, tStart, tEnd, tCompute, tAvg, tMin, tMax;
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
    // init[0]=1;
    // for (i = 0; i < 10; i++)
    // {
    //     MPI_Bcast(&init, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    //     MPI_Gather(&init, 1, MPI_INT, &initbuf, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    //     // if (rank == MASTER) {
    //     //     for (i = 1; i < numtasks; i++)
    //     //     {
    //     //         MPI_Send(&init, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &sendRequest);
    //     //     }
    //     //     for (i = 1; i < numtasks; i++)
    //     //     {
    //     //         MPI_Recv(&init, 1, MPI_INT, i, 6, MPI_COMM_WORLD, &status);
    //     //     }
    //     // } else {
    //     //     MPI_Recv(&init, 1, MPI_INT, MASTER, 5, MPI_COMM_WORLD, &status);
    //     //     MPI_Send(&init, 1, MPI_INT, MASTER, 6, MPI_COMM_WORLD);
    //     // }
    // }
    
    for (i = 0; i < 256; i++) {
        histbuf[i] = 0;
        histbuf2[i] = 0;
    }
    if (rank == MASTER)
    {
        for (i = 0; i < 256; i++) {
            finalbuf[i] = 0;
            finalbuf2[i] = 0;
            sendbuf[i] = (unsigned char)(rand()%256);
            // testbuf[i] = 0;
        }
        for (; i < ARRAYSIZE; i++) {
            sendbuf[i] = (unsigned char)(rand()%256);
        }
    }
    for (i = 0; i < 100; i++)
    {
        // if (rank == MASTER)
        // {
        //     /* code */
        // }
        
        MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    }
    

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == MASTER) {
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
        // globStart = MPI_Wtime();
        scatterStart = MPI_Wtime();
    }
    for (i = 0; i < ITERATIONS; i++)
    {
        MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    }
    if (rank == MASTER) {
        scatterEnd = MPI_Wtime();
    }
    // tStart = MPI_Wtime();
    if (rank == MASTER) {
        for (i = 0; i < leftover; i++) {
            histbuf[sendbuf[i]]++;
        }
    }
    for (i = 0; i < chunksize; i++) {
        histbuf[recvbuf[i]]++;
    }
    // tEnd = MPI_Wtime();
    if (rank == MASTER) {
        gatherStart = MPI_Wtime();
    }
    for (i = 0; i < ITERATIONS; i++)
    {
        MPI_Gather(&histbuf, 256, MPI_INT, &gatherbuf, 256, MPI_INT, MASTER, MPI_COMM_WORLD);
    }
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

        // globEnd = MPI_Wtime();
        // printf("Total: %f\tScatter: %f\tGather: %f\n", globEnd-globStart, scatterEnd-scatterStart, gatherEnd-gatherStart);
        // printf("%04d %f %f %f ", numtasks, globEnd-globStart, (scatterEnd-scatterStart)/20, gatherEnd-gatherStart);
        printf("%04d %f %f ", numtasks, (scatterEnd-scatterStart)/ITERATIONS, (gatherEnd-gatherStart)/ITERATIONS);
    }
    // tCompute = tEnd-tStart;
    // MPI_Gather(&tCompute, 1, MPI_DOUBLE, timebuf, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
    // if (rank == MASTER)
    // {
    //     tAvg = 0;
    //     tMin = 999999.9;
    //     tMax = 0.0;

    //     for (i = 0; i < numtasks; i++) {
    //         tAvg += timebuf[i];
    //         if (timebuf[i]>tMax) {
    //             tMax = timebuf[i];
    //         }
    //         if (timebuf[i]<tMin) {
    //             tMin = timebuf[i];
    //         }
    //     }
    //     tAvg = tAvg/numtasks;
    //     // printf("Compute Min: %f\tAvg: %f\tMax: %f\n", tMin, tAvg, tMax);
    // }

    // for (i = 0; i < 256; i++) {
    //     histbuf[i] = 0;
    // }

    /* Master Task*/
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == MASTER)
    {
        // for (i = 0; i < 256; i++) {
        //     histbuf[i] = 0;
        // }
        // globStart2 = MPI_Wtime();
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
        // printf("\n");
        // globEnd2 = MPI_Wtime();
        // printf("Total: %f\tSend: %f\tRecv: %f\n", globEnd2-globStart2, sendEnd-sendStart, recvEnd-recvStart);
        // printf("%f %f %f\n", numtasks, globEnd2-globStart2, sendEnd-sendStart, recvEnd-recvStart);
        printf("%f %f\n", (sendEnd-sendStart)/ITERATIONS, (recvEnd-recvStart)/ITERATIONS);

        // for (int i = 0; i < 256; i++)
        // {
        //     if (finalbuf[i]!=finalbuf2[i])
        //     {
        //         printf("OOPS!\n");
        //     }
            
        //     // printf("%i ", finalbuf[i]);
        // }
        // printf("\n");

    } 
    /* Worker Tasks */
    else {
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