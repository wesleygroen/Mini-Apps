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
#define DARTS 10000000     /* number of throws at dartboard */
#define ROUNDS 10000 
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[])
{
   double	homepi,         /* value of pi calculated by current task */
      pisum,	        /* sum of tasks' pi values */
      // pi,	        /* average of pi after "darts" is thrown */
      startT, endT;
      // avepi;	        /* average pi value for all iterations */
   int	taskid,	        /* task ID - also used as seed number */
      numtasks,       /* number of tasks */
      dart;

   /* Obtain number of tasks and task ID */
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   startT = 0.0;
   if (taskid == MASTER){
      startT = MPI_Wtime();
   }

   /* Set seed for random number generator equal to task ID */
   srandom(taskid);
   dart = (int)floor(DARTS/numtasks);

   /* All tasks calculate pi using dartboard algorithm */
   homepi = dboard(dart, ROUNDS);

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
   MPI_Reduce(&homepi, &pisum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD); 

   if (taskid == MASTER){
      endT = MPI_Wtime();
      printf("%04d %f\n", numtasks, endT-startT);
   }
   MPI_Finalize();
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
   long random(void);
   double x_coord, y_coord, pi, r; 
   int n, i;
   unsigned long score;// temp;
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
   /* 2 bit shifted to MAX_RAND later used to scale random number between 0 and 1 */
   cconst = 2 << (31 - 1);
   score = 0;
   // temp = 0;

   /* "throw darts at board" */
   for (i = 1; i <= rounds; i++) {
      for (n = 1; n <= darts; n++) {
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
   pi = 4.0 * (double)score/(double)darts/(double)rounds;
   return(pi);
} 
