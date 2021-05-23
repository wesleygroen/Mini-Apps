/****************************************************************************
* FILE: mpi_bandwidth.c
* DESCRIPTION:
*   Provides point-to-point communications timings for any even
*   number of MPI tasks.
* AUTHOR: Blaise Barney
* LAST REVISED: 04/13/05
****************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
// #include <sys/time.h>
// #include <time.h>

#define MAXTASKS      8192
/* Change the next four parameters to suit your case */
#define STARTSIZE     100000
// #define ENDSIZE       134217728
#define ENDSIZE       100000000
// #define ENDSIZE    268435456
// #define INCREMENT     100000
#define ROUNDTRIPS    20
#define TESTS         4

int compareArrays(double a[], double b[], int n) {
    int i;
    for(i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            printf("\n# %d | %f | %f #\n", i, a[i], b[i]);
            return 0;
        }
        // better:
        // if(fabs(a[i]-b[i]) < 1e-10 * (fabs(a[i]) + fabs(b[i]))) {
        // with the appropriate tolerance
    }
    return 1;
}

int main (int argc, char *argv[])
{
    int     numtasks, rank, n, i, j, rndtrps, start, end, numpairs,
            src, dest, tag=1, taskpairs[MAXTASKS], namelength, sizes[TESTS];
    double  sendbuf[ENDSIZE], recvbuf[ENDSIZE], thistime, bw, bestbw, worstbw, totalbw, avgbw,
            bestall[TESTS], avgall[TESTS], worstall[TESTS], best[TESTS], worst[TESTS],
            timings[MAXTASKS/2][3], tmptimes[3], t1, t2;
    char    host[MPI_MAX_PROCESSOR_NAME],
            hostmap[MAXTASKS][MPI_MAX_PROCESSOR_NAME];
    unsigned long long nbytes;
    unsigned int seed;
    // int incr;
    // double resolution;
    // struct 	timeval tv1, tv2;
    MPI_Status status;

    /* Some initializations and error checking */
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    if (numtasks % 2 != 0) {
        printf("ERROR: Must be an even number of tasks!  Quitting...\n");
        MPI_Abort(MPI_COMM_WORLD, MPI_ERR_DIMS);
        exit(0);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    start = STARTSIZE;
    end = ENDSIZE;
    rndtrps = ROUNDTRIPS;
    numpairs = numtasks/2;
    seed = 42;
    srand(seed);
    sizes[0] = 1;
    sizes[1] = 1000;
    sizes[2] = start;
    sizes[3] = end;
    for (i=0; i<end; i++){
        sendbuf[i] = (double)rand()/RAND_MAX;
    }

    /* All tasks send their host name to task 0 */
    MPI_Get_processor_name(host, &namelength);
    MPI_Gather(&host, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, &hostmap, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, MPI_COMM_WORLD);

    /* Determine who my send/receive partner is and tell task 0 */
    if (rank < numpairs)
        dest = src = numpairs + rank;
    if (rank >= numpairs)
        dest = src = rank - numpairs;
    MPI_Gather(&dest, 1, MPI_INT, &taskpairs, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // if (rank == 0) {
    //     resolution = MPI_Wtick();
    //     printf("\n******************** MPI Bandwidth Test ********************\n");
    //     printf("Message start size= %d bytes\n",start);
    //     printf("Message finish size= %d bytes\n",end);
    //     printf("Incremented by %d bytes per iteration\n",incr);
    //     printf("Roundtrips per iteration= %d\n",rndtrps);
    //     printf("MPI_Wtick resolution = %e\n",resolution);
    //     printf("************************************************************\n");
    //     // for (i=0; i<numtasks; i++)
    //     //     printf("task %4d is on %s partner=%4d\n",i,hostmap[i],taskpairs[i]);
    //     // printf("************************************************************\n");
    // }


    /*************************** first half of tasks *****************************/
    /* These tasks send/receive messages with their partner task, and then do a  */
    /* few bandwidth calculations based upon message size and timings.           */

    if (rank < numpairs) {
        for (int test=0; test<TESTS; test++) {
            n = sizes[test];
            bestbw = 0.0;
            worstbw = .99E+99;
            totalbw = 0.0;
            nbytes =  sizeof(double) * n;
            MPI_Barrier(MPI_COMM_WORLD);
            for (i=1; i<=rndtrps; i++){
                t1 = MPI_Wtime();
                MPI_Ssend(&sendbuf, n, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
                MPI_Recv(&recvbuf, n, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &status);
                t2 = MPI_Wtime();
                thistime = t2 - t1;
                bw = ((double)nbytes * 2) / thistime;
                totalbw = totalbw + bw;
                // printf("%.4f\n", thistime);
                if (bw > bestbw ){
                    bestbw = bw;
                }
                if (bw < worstbw ){
                    worstbw = bw;
                }
            }
            if (!compareArrays(sendbuf, recvbuf, n))
            {
                printf("Send and receive buffer are not equal!\n");
            }
           
            /* Convert to megabytes per second */
            bestbw = bestbw/1000000.0;
            avgbw = (totalbw/1000000.0)/(double)rndtrps;
            worstbw = worstbw/1000000.0;

            /* Task 0 collects timings from all relevant tasks */
            if (rank == 0) {
                /* Keep track of my own timings first */
                timings[0][0] = bestbw;   
                timings[0][1] = avgbw;   
                timings[0][2] = worstbw;   
                /* Initialize overall averages */
                bestall[test] = 0.0;
                avgall[test] = 0.0;
                worstall[test] = 0.0;
                best[test] = 0.0;
                worst[test] = 10e99;
                /* Now receive timings from other tasks and print results. Note */
                /* that this loop will be appropriately skipped if there are    */
                /* only two tasks. */
                for (j=1; j<numpairs; j++)
                {
                    MPI_Recv(&timings[j], 3, MPI_DOUBLE, j, tag, MPI_COMM_WORLD, &status);
                }
                // printf("***Message size: %8d *** best  /  avg  / worst (MB/sec)\n",(int)(n*sizeof(double)));
                for (j=0; j<numpairs; j++)
                {
                    // printf("   task pair: %4d - %4d:    %04.2f / %04.2f / %04.2f \n", j, taskpairs[j], timings[j][0], timings[j][1], timings[j][2]);
                    bestall[test] += timings[j][0];
                    avgall[test] += timings[j][1];
                    worstall[test] += timings[j][2];
                    // printf("%04.2f / %04.2f / %04.2f / %04.2f / %04.2f \n", timings[j][0], timings[j][1], timings[j][2], best, worst);
                    if (timings[j][0] > best[test])
                        best[test] = timings[j][0];
                    if (timings[j][2] < worst[test])
                        worst[test] = timings[j][2];
                }
                bestall[test] = bestall[test]/numpairs;
                avgall[test] =  avgall[test]/numpairs;
                worstall[test] = worstall[test]/numpairs;
                // printf("   OVERALL AVERAGES:          %04.2f / %04.2f / %04.2f \n", bestall[test], avgall[test], worstall[test]);
                // printf("   ABSOLUTE MAX/MIN:          %04.2f /         / %04.2f \n", best[test], worst[test]);
            }
            else
            {
                /* Other tasks send their timings to task 0 */
                tmptimes[0] = bestbw;   
                tmptimes[1] = avgbw;   
                tmptimes[2] = worstbw;   
                MPI_Send(tmptimes, 3, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
            }
        }
        if (rank == 0)
        {
            // char *dataSize;
            printf("%04d", numtasks);
            for (int test = 0; test < TESTS; test++)
            {
                printf(" %f %f %f", best[test], avgall[test], worst[test]);
                // printf(" %f %f %f %f %f", bestall[test], avgall[test], worstall[test], best[test], worst[test]);
                // if      (test == 0) {dataSize = "  8B ";} 
                // else if (test == 1) {dataSize = "  8KB";} 
                // else if (test == 2) {dataSize = "800KB";} 
                // else if (test == 3) {dataSize = "800MB";} 
                // printf("%04d %s %f %f %f %f %f\n", numtasks, dataSize, bestall[test], avgall[test], worstall[test], best[test], worst[test]);
                // printf("   OVERALL AVERAGES:          %04.2f / %04.2f / %04.2f \n", bestall[test], avgall[test], worstall[test]);
                // printf("   ABSOLUTE MAX/MIN:          %04.2f /         / %04.2f \n", best[test], worst[test]);
            }
            printf("\n");
        }
        
    }

    /**************************** second half of tasks ***************************/
    /* These tasks do nothing more than send and receive with their partner task */

    if (rank >= numpairs)
    {
        for (int test=0; test<TESTS; test++) {
            n = sizes[test];
            // n = end;
            MPI_Barrier(MPI_COMM_WORLD);
            for (i=1; i<=rndtrps; i++){
                MPI_Recv(&recvbuf, n, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &status);
                MPI_Send(&recvbuf, n, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();
}  /* end of main */