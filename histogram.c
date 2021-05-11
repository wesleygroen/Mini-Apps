/******************************************************************************
* FILE: mpi_scatter.c
* DESCRIPTION:
*   MPI tutorial example code: Collective Communications
* AUTHOR: Blaise Barney
* LAST REVISED: 04/13/05
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define WIDTH 9000
#define HEIGHT 7000
#define ARRAYSIZE WIDTH*HEIGHT
#define MASTER 0        /* task ID of master task */
// #define 

int main (int argc, char *argv[])
{
    int numtasks, rank, sendcount, recvcount, source, offset, chunksize, leftover, tag1, tag2, status, dest, sum, mysum;
    unsigned char sendbuf[HEIGHT * WIDTH];
    //  = {
    //     {1.0, 2.0, 3.0, 4.0},
    //     {5.0, 6.0, 7.0, 8.0},
    //     {9.0, 10.0, 11.0, 12.0},
    //     {13.0, 14.0, 15.0, 16.0}
    // };
    int histbuf[256];
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    // int seed = 42;
    srand(42);
    chunksize = (ARRAYSIZE / numtasks);
    leftover = (ARRAYSIZE % numtasks);
    tag2 = 1;
    tag1 = 2;
    for (int i = 0; i < 256; i++) {
        histbuf[i] = 0;
    }
    if (rank == MASTER)
    {
        for (int h = 0; h < ARRAYSIZE; h++) {
            sendbuf[h] = (unsigned char)(rand()%256);
            // for (int w = 0; w < WIDTH; w++) {
            //     sendbuf[h][w][0] = (unsigned char)(rand()%256);
            //     sendbuf[h][w][1] = (unsigned char)(rand()%256);
            //     sendbuf[h][w][2] = (unsigned char)(rand()%256); 
            // }
        }
        offset = chunksize + leftover;
        for (dest = 1; dest < numtasks; dest++) {
            MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            MPI_Send(&sendbuf[offset], chunksize, MPI_DOUBLE, dest, tag2, MPI_COMM_WORLD);
            printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
            offset = offset + chunksize;
        }
        
        // for (int i = 0; i < HEIGHT * WIDTH; i++)
        // {
        //     histbuf[sendbuf[i]]++;
        // }
        
        // printf("\n");
        // for (int i = 0; i < 256; i++)
        // {
        //     printf("%i ", histbuf[i]);
        // }
        // printf("\n");
    }

/***** Non-master tasks only *****/

    if (rank > MASTER) {

        /* Receive my portion of array from the master task */
        source = MASTER;
        MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
        MPI_Recv(&sendbuf[offset], chunksize, MPI_DOUBLE, source, tag2, MPI_COMM_WORLD, &status);

        /* Do my part of the work */
        mysum = update(offset, chunksize, rank);

        /* Send my results back to the master task */
        dest = MASTER;
        MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
        MPI_Send(&sendbuf[offset], chunksize, MPI_DOUBLE, MASTER, tag2, MPI_COMM_WORLD);

        /* Use sum reduction operation to obtain final sum */
        MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    } /* end of non-master */

    MPI_Finalize();
}