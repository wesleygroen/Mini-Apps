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
    int numtasks, rank, chunksize, leftover, i;
    unsigned char sendbuf[HEIGHT * WIDTH];
    int histbuf[256], finalbuf[256];
    // int testbuf[256];
    // MPI_Status status;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    int gatherbuf[256*numtasks];
    srand(42);
    chunksize = (ARRAYSIZE / numtasks);
    leftover = (ARRAYSIZE % numtasks);
    unsigned char recvbuf[chunksize];

    for (i = 0; i < 256; i++) {
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
        for (i = 0; i < 256; i++) {
            finalbuf[i] = 0;
            // testbuf[i] = 0;
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
    MPI_Scatter(&sendbuf[leftover], chunksize, MPI_CHAR, &recvbuf, chunksize, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    if (rank == MASTER) {
        for (i = 0; i < leftover; i++) {
            histbuf[sendbuf[i]]++;
        }
    }
    for (i = 0; i < chunksize; i++) {
        histbuf[recvbuf[i]]++;
    }
    
    MPI_Gather(&histbuf, 256, MPI_INT, &gatherbuf, 256, MPI_INT, MASTER, MPI_COMM_WORLD);

    if (rank == MASTER)
    {
        for (i = 0; i < 256*numtasks; i++)
        {            
            // printf("%d + %d = ",finalbuf[i%256], gatherbuf[i]);
            finalbuf[i%256]+=gatherbuf[i];
            // printf("%d\n", finalbuf[i%256]);
        }
        printf("\n");
        for (int i = 0; i < 256; i++)
        {
            printf("%i ", finalbuf[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
}