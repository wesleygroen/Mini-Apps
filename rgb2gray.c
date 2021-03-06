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
    int numtasks, rank, chunksize, leftover, i, j, index, tag;// init[1];//, msgsize;
    unsigned char red[ARRAYSIZE], green[ARRAYSIZE], blue[ARRAYSIZE], gray[ARRAYSIZE];
    double sendStart, sendEnd, recvStart, recvEnd;
    double tCompute, tStart, tEnd, tAvg, tMin, tMax;
    MPI_Status status;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    double timebuf[numtasks];
    srand(42);
    sendStart = sendEnd = recvStart = recvEnd= 0.0;
    chunksize = (ARRAYSIZE / numtasks);
    unsigned char redbuf[chunksize], greenbuf[chunksize], bluebuf[chunksize], graybuf[chunksize];
    leftover = (ARRAYSIZE % numtasks);
    unsigned char recvbuf[chunksize];
    
    // if (rank == MASTER)
    // {
    //     for (i = 0; i < ARRAYSIZE; i++) {
    //         red[i] = (unsigned char)(rand()%256);
    //         green[i] = (unsigned char)(rand()%256);
    //         blue[i] = (unsigned char)(rand()%256);  
    //     }
    // }
    // for (i = 0; i < 100; i++)
    // {
    // MPI_Scatter(&red[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    // }
    // MPI_Barrier(MPI_COMM_WORLD);
    /* Master Task*/
    // MPI_Barrier(MPI_COMM_WORLD);
    if (rank == MASTER)
    {
        for (i = 0; i < ARRAYSIZE; i++) {
            red[i] = (unsigned char)(rand()%256);
            green[i] = (unsigned char)(rand()%256);
            blue[i] = (unsigned char)(rand()%256);
        }
        MPI_Scatter(&red[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        sendStart = MPI_Wtime();
        for (j = 0; j < ITERATIONS; j++)
        {
            index = leftover + chunksize;

            for (i = 1; i < numtasks; i++)
            {
                MPI_Send(&red[index], chunksize, MPI_CHAR, i, 1, MPI_COMM_WORLD);
                MPI_Send(&green[index], chunksize, MPI_CHAR, i, 2, MPI_COMM_WORLD);
                MPI_Send(&blue[index], chunksize, MPI_CHAR, i, 3, MPI_COMM_WORLD);
                MPI_Send(&i, 1, MPI_INT, i, 4, MPI_COMM_WORLD);
                index += chunksize;
            }
        }
        sendEnd = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        tStart = MPI_Wtime();
        for (i = 0; i < leftover + chunksize; i++)
        {
            gray[i]=(char)(0.299*(float)red[i]+0.587*(float)green[i]+0.114*(float)blue[i]);
        }
        tEnd = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        recvStart = MPI_Wtime();
        for (j = 0; j < ITERATIONS; j++)
        {
            index = leftover + chunksize;
            for (i = 1; i < numtasks; i++)
            {
                MPI_Recv(&gray[index], chunksize, MPI_CHAR, i, i, MPI_COMM_WORLD, &status);
                index += chunksize;
            }
        }
        recvEnd = MPI_Wtime();
        // printf("%04d %f %f\n", numtasks, (sendEnd-sendStart)/ITERATIONS, (recvEnd-recvStart)/ITERATIONS);
        tCompute = tEnd - tStart;
        MPI_Gather(&tCompute, 1, MPI_DOUBLE, timebuf, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

        tAvg = 0.0;
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
        // printf("Compute Min: %f\tAvg: %f\tMax: %f\n", tMin, tAvg, tMax);
        printf("%04d %f %f ", numtasks, (sendEnd-sendStart)/ITERATIONS, (recvEnd-recvStart)/ITERATIONS);
        printf("%f %f %f\n", tMin, tAvg, tMax);
    } 
    /* Worker Tasks */
    else {
        MPI_Scatter(&red[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        for (j = 0; j < ITERATIONS; j++)
        {
            MPI_Recv(&redbuf, chunksize, MPI_CHAR, MASTER, 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&greenbuf, chunksize, MPI_CHAR, MASTER, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&bluebuf, chunksize, MPI_CHAR, MASTER, 3, MPI_COMM_WORLD, &status);
            MPI_Recv(&tag, 1, MPI_INT, MASTER, 4, MPI_COMM_WORLD, &status);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        tStart = MPI_Wtime();
        for (i = 0; i < chunksize; i++)
        {
            graybuf[i]=(char)(0.299*(float)redbuf[i]+0.587*(float)greenbuf[i]+0.114*(float)bluebuf[i]);
        }
        tEnd = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        for (j = 0; j < ITERATIONS; j++)
        {
            MPI_Send(&graybuf, chunksize, MPI_CHAR, MASTER, rank, MPI_COMM_WORLD);
        }
        tCompute = tEnd - tStart;
        MPI_Gather(&tCompute, 1, MPI_DOUBLE, timebuf, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
    }

    // tCompute = tEnd - tStart;
    // MPI_Gather(&tCompute, 1, MPI_DOUBLE, timebuf, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
    // if (rank == MASTER)
    // {
    //     tAvg = 0.0;
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
    //     printf("%04d %f %f ", numtasks, (sendEnd-sendStart)/ITERATIONS, (recvEnd-recvStart)/ITERATIONS);
    //     printf("%f %f %f\n", tMin, tAvg, tMax);
    // }
    MPI_Finalize();
}