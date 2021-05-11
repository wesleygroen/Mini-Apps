/**********************************************************************
 * FILE: mpi_pi_reduce.c
 * OTHER FILES: dboard.c
 * DESCRIPTION:  
 *   MPI pi Calculation Example - C Version 
 *   Collective Communication example:  
 *   This program calculates pi using a "dartboard" algorithm.  See
 *   Fox et al.(1988) Solving Problems on Concurrent Processors, vol.1
 *   page 207.  All processes contribute to the calculation, with the
 *   master averaging the values for pi. This version uses mpc_reduce to 
 *   collect results
 * AUTHOR: Blaise Barney. Adapted from Ros Leibensperger, Cornell Theory
 *   Center. Converted to MPI: George L. Gusciora, MHPCC (1/95) 
 * LAST REVISED: 06/13/13 Blaise Barney
**********************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

void srandom (unsigned seed);
double dboard (int darts, int rounds);
#define DARTS 100000     /* number of throws at dartboard */
#define ROUNDS 100      /* number of times "darts" is iterated */
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[])
{
   // struct timeval begin, end;
   double	homepi,         /* value of pi calculated by current task */
      tsum,	        /* sum of tasks' pi values */
      t,	        /* average of pi after "darts" is thrown */
      avepi,	        /* average pi value for all iterations */
      avet;
   int	taskid,	        /* task ID - also used as seed number */
      numtasks,       /* number of tasks */
      rc,             /* return code */
      i;
      // dart;
   
   MPI_Status status;
   // if (taskid == MASTER){
   //    gettimeofday(&begin, 0);
   // }
   /* Obtain number of tasks and task ID */
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   // printf ("MPI task %d has started...\n", taskid);

   /* Set seed for random number generator equal to task ID */
   srandom (taskid);
   // dart = (int)floor(DARTS/numtasks);
   // dart = DARTS;
   avet = 0;
   // for (i = 0; i < ROUNDS; i++) {
      /* All tasks calculate pi using dartboard algorithm */
      homepi = dboard(DARTS, ROUNDS);

      /* Use MPI_Reduce to sum values of homepi across all tasks 
      * Master will store the accumulated value in pisum 
      * - homepi is the send buffer
      * - pisum is the receive buffer (used by the receiving task only)
      * - the size of the message is sizeof(double)
      * - MASTER is the task that will receive the result of the reduction
      *   operation
      * - MPI_SUM is a pre-defined reduction function (double-precision
      *   floating-point vector addition).  Must be declared extern.
      * - MPI_COMM_WORLD is the group of tasks that will participate.
      */

      rc = MPI_Reduce(&homepi, &tsum, 1, MPI_DOUBLE, MPI_SUM,
                     MASTER, MPI_COMM_WORLD);
      // printf("%f\n", tsum);

      /* Master computes average for this iteration and all iterations */
      if (taskid == MASTER) {
         t = tsum/numtasks;
         avet = ((avet * i) + t)/(i + 1); 
         // printf("%f %f\n", t, avet);
         // printf("   After %8d throws, average value of pi = %10.8f\n",(DARTS * (i + 1)),avepi);
      }    
   // } 
   MPI_Finalize();
   if (taskid == MASTER){
      // gettimeofday(&end, 0);
      // long sec = end.tv_sec - begin.tv_sec;
      // long msec = end.tv_usec - begin.tv_usec;
      // double runtime = sec + msec*1e-6;
      printf("\nm: %lf\n", avet);
      // printf("%.6f\n", runtime);
      // printf("runtime: %.6f sec\n", runtime);
      // printf("runtime: %lu %lu sec\n", end.tv_sec, begin.tv_sec);
      // printf ("\nReal value of PI: 3.1415926535897 \n");
   }
   return 0;
}



/**************************************************************************
* subroutine dboard
* DESCRIPTION:
*   Used in pi calculation example codes. 
*   See mpi_pi_send.c and mpi_pi_reduce.c  
*   Throw darts at board.  Done by generating random numbers 
*   between 0 and 1 and converting them to values for x and y 
*   coordinates and then testing to see if they "land" in 
*   the circle."  If so, score is incremented.  After throwing the 
*   specified number of darts, pi is calculated.  The computed value 
*   of pi is returned as the value of this function, dboard. 
*
*   Explanation of constants and variables used in this function:
*   darts       = number of throws at dartboard
*   score       = number of darts that hit circle
*   n           = index variable
*   r           = random number scaled between 0 and 1
*   x_coord     = x coordinate, between -1 and 1
*   x_sqr       = square of x coordinate
*   y_coord     = y coordinate, between -1 and 1
*   y_sqr       = square of y coordinate
*   pi          = computed value of pi
****************************************************************************/

double dboard(int darts, int rounds)
{
   #define sqr(x)	((x)*(x))
   struct timeval begin, end;
   long random(void);
   double x_coord, y_coord, pi, r; 
   int score, n, i;
   unsigned int cconst;  /* must be 4-bytes in size */
   /*************************************************************************
    * The cconst variable must be 4 bytes. We check this and bail if it is
    * not the right size
    ************************************************************************/
   if (sizeof(cconst) != 4) {
      printf("Wrong data size for cconst variable in dboard routine!\n");
      printf("See comments in source file. Quitting.\n");
      exit(1);
   }
   
   gettimeofday(&begin, 0);

   /* 2 bit shifted to MAX_RAND later used to scale random number between 0 and 1 */
   cconst = 2 << (31 - 1);
   score = 0;
   for (i = 0; i < rounds; i++)
   {
      /* "throw darts at board" */
      for (n = 1; n <= darts; n++)  {
         /* generate random numbers for x and y coordinates */
         r = (double)random()/cconst;
         x_coord = (2.0 * r) - 1.0;
         r = (double)random()/cconst;
         y_coord = (2.0 * r) - 1.0;

         /* if dart lands in circle, increment score */
         if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
            score++;
      }
   }
   
   /* calculate pi */
   pi = 4.0 * (double)score/((double)darts*(double)rounds);
   
   gettimeofday(&end, 0);
   long sec = end.tv_sec - begin.tv_sec;
   long msec = end.tv_usec - begin.tv_usec;
   double runtime = sec + msec*1e-6;
   printf("%f\n", runtime);
   return(runtime);
} 



