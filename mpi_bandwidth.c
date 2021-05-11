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
#define ENDSIZE       134217728
// #define ENDSIZE    268435456
#define INCREMENT     100000
#define ROUNDTRIPS    100
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
    int     numtasks, rank, n, i, j, rndtrps, start, end, incr,
            src, dest, rc, tag=1, taskpairs[MAXTASKS], namelength, sizes[TESTS];
    double  sendbuf[ENDSIZE], recvbuf[ENDSIZE], thistime, bw, bestbw, worstbw, totalbw, avgbw,
            bestall, avgall, worstall, best, worst,
            timings[MAXTASKS/2][3], tmptimes[3],
            resolution, t1, t2;
    char    host[MPI_MAX_PROCESSOR_NAME],
            hostmap[MAXTASKS][MPI_MAX_PROCESSOR_NAME];
    unsigned long long nbytes;
    unsigned int seed;
    // struct 	timeval tv1, tv2;
    MPI_Status status;

    /* Some initializations and error checking */
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    if (numtasks % 2 != 0) {
        printf("ERROR: Must be an even number of tasks!  Quitting...\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(0);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    start = STARTSIZE;
    end = ENDSIZE;
    incr = INCREMENT;
    rndtrps = ROUNDTRIPS;
    seed = 42;
    srand(seed);
    sizes[0] = 1;
    sizes[1] = 1000;
    sizes[2] = start;
    sizes[3] = end;
    for (i=0; i<end; i++){
        sendbuf[i] = (double)rand()/RAND_MAX;
        // msgbuf[i] = 1.79769e+300;
    }

    /* All tasks send their host name to task 0 */
    MPI_Get_processor_name(host, &namelength);
    MPI_Gather(&host, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, &hostmap,
        MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, MPI_COMM_WORLD);

    /* Determine who my send/receive partner is and tell task 0 */
    if (rank < numtasks/2) 
        dest = src = numtasks/2 + rank;
    if (rank >= numtasks/2) 
        dest = src = rank - numtasks/2;
    MPI_Gather(&dest, 1, MPI_INT, &taskpairs, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        resolution = MPI_Wtick();
        printf("\n******************** MPI Bandwidth Test ********************\n");
        printf("Message start size= %d bytes\n",start);
        printf("Message finish size= %d bytes\n",end);
        printf("Incremented by %d bytes per iteration\n",incr);
        printf("Roundtrips per iteration= %d\n",rndtrps);
        printf("MPI_Wtick resolution = %e\n",resolution);
        printf("************************************************************\n");
        for (i=0; i<numtasks; i++) 
            printf("task %4d is on %s partner=%4d\n",i,hostmap[i],taskpairs[i]);
        printf("************************************************************\n");
    }


    /*************************** first half of tasks *****************************/
    /* These tasks send/receive messages with their partner task, and then do a  */ 
    /* few bandwidth calculations based upon message size and timings.           */

    if (rank < numtasks/2) {
        for (int ni=0; ni<TESTS; ni++) {
            n = sizes[ni];
            // n = start;
            bestbw = 0.0;
            worstbw = .99E+99;
            totalbw = 0.0;
            nbytes =  sizeof(double) * n;
            printf("%d\n", nbytes);
            for (i=1; i<=rndtrps; i++){
                t1 = MPI_Wtime();
                MPI_Ssend(&sendbuf, n, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
                MPI_Recv(&recvbuf, n, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &status);
                t2 = MPI_Wtime();
                thistime = t2 - t1;
                bw = ((double)nbytes * 2) / thistime;
                totalbw = totalbw + bw;
                // printf("%.4f\n", thistime);
                if (bw > bestbw )
                    bestbw = bw;
                if (bw < worstbw )
                    worstbw = bw;
            }
            if (!compareArrays(sendbuf, recvbuf, n))
            {
                printf("Send and receive buffer are not equal!\n");
            }
            
            /* Convert to megabytes per second */
            bestbw = bestbw/1048576.0;
            avgbw = (totalbw/1048576.0)/(double)rndtrps;
            worstbw = worstbw/1048576.0;

            /* Task 0 collects timings from all relevant tasks */
            if (rank == 0) {
                /* Keep track of my own timings first */
                timings[0][0] = bestbw;    
                timings[0][1] = avgbw;    
                timings[0][2] = worstbw;    
                /* Initialize overall averages */
                bestall = 0.0;
                avgall = 0.0;
                worstall = 0.0;
                best = 0.0;
                worst = 10e99;
                /* Now receive timings from other tasks and print results. Note */
                /* that this loop will be appropriately skipped if there are    */
                /* only two tasks. */
                for (j=1; j<numtasks/2; j++)
                    MPI_Recv(&timings[j], 3, MPI_DOUBLE, j, tag, MPI_COMM_WORLD, &status);
                printf("***Message size: %8d *** best  /  avg  / worst (MB/sec)\n",n);
                for (j=0; j<numtasks/2; j++) { 
                    printf("   task pair: %4d - %4d:    %04.2f / %04.2f / %04.2f \n",
                        j, taskpairs[j], timings[j][0], timings[j][1], timings[j][2]);
                    bestall += timings[j][0];
                    avgall += timings[j][1];
                    worstall += timings[j][2];
                    // printf("%04.2f / %04.2f / %04.2f / %04.2f / %04.2f \n", timings[j][0], timings[j][1], timings[j][2], best, worst);
                    if (timings[j][0] > best)
                        best = timings[j][0];
                    if (timings[j][2] < worst)
                        worst = timings[j][2];
                }
                printf("   OVERALL AVERAGES:          %04.2f / %04.2f / %04.2f \n",
                    bestall/(numtasks/2), avgall/(numtasks/2), worstall/(numtasks/2));
                printf("   ABSOLUTE MAX/MIN:          %04.2f /         / %04.2f \n", best, worst);
            }
            else {
                /* Other tasks send their timings to task 0 */
                tmptimes[0] = bestbw;    
                tmptimes[1] = avgbw;    
                tmptimes[2] = worstbw;    
                MPI_Send(tmptimes, 3, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
            }
        }
    }



    /**************************** second half of tasks ***************************/
    /* These tasks do nothing more than send and receive with their partner task */

    if (rank >= numtasks/2) {
        // for (n=start; n<=end; n=n+incr) {
        for (int ni=0; ni<TESTS; ni++) {
            n = sizes[ni];
            // n = end;
            for (i=1; i<=rndtrps; i++){
                MPI_Recv(&recvbuf, n, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &status);
                MPI_Send(&recvbuf, n, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
            }
        }
    }


    MPI_Finalize();

}  /* end of main */