/******************************************************************************
* FILE: mpi_mm.c
* DESCRIPTION:  
*   MPI Matrix Multiply - C Version
*   In this code, the master task distributes a matrix multiply
*   operation to numtasks-1 worker tasks.
*   NOTE:  C and Fortran versions of this code differ because of the way
*   arrays are stored/passed.  C arrays are row-major order but Fortran
*   arrays are column-major order.
* AUTHOR: Blaise Barney. Adapted from Ros Leibensperger, Cornell Theory
*   Center. Converted to MPI: George L. Gusciora, MHPCC (1/95)
* LAST REVISED: 04/13/05
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#define NRA 4096               /* number of rows in matrix A */
#define NCA 4096               /* number of columns in matrix A */
#define NCB 4096               /* number of columns in matrix B */
#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

int main (int argc, char *argv[])
{
	int	numtasks,              /* number of tasks in partition */
		taskid,                /* a task identifier */
		numworkers,            /* number of worker tasks */
		source,                /* task id of message source */
		dest,                  /* task id of message destination */
		mtype,                 /* message type */
		rows,                  /* rows of matrix A sent to each worker */
		averow, extra, offset, /* used to determine rows sent to each worker */
		i, j, k;           	   /* misc */
	double	a[NRA][NCA],       /* matrix A to be multiplied */
			b[NCA][NCB],       /* matrix B to be multiplied */
			c[NRA][NCB],       /* result matrix C */
			start, end, t, best, avg, worst,
			sendStart, sendEnd;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	if (numtasks < 2 ) {
		printf("Need at least two MPI tasks. Quitting...\n");
		MPI_Abort(MPI_COMM_WORLD, 2);
		exit(1);
	}
	numworkers = numtasks-1;
	double timebuf[numtasks];

	MPI_Barrier(MPI_COMM_WORLD);
	/**************************** master task ************************************/
	if (taskid == MASTER)
	{
		start = MPI_Wtime();
		for (i=0; i<NRA; i++)
		{
			for (j=0; j<NCA; j++) {
				a[i][j]= i+j;
			}
		}
		for (i=0; i<NCA; i++)
		{
			for (j=0; j<NCB; j++)
			{
				b[i][j]= i*j;
			}
		}

		/* Send matrix data to the worker tasks */
		averow = NRA/numworkers;
		extra = NRA%numworkers;
		offset = 0;
		mtype = FROM_MASTER;
		sendStart = MPI_Wtime();
		MPI_Bcast(&b, NCA*NCB, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
		for (dest=1; dest<=numworkers; dest++)
		{
			rows = (dest <= extra) ? averow+1 : averow;   	
			MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&a[offset][0], rows*NCA, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
			// MPI_Send(&b, NCA*NCB, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
			offset = offset + rows;
		}
		sendEnd = MPI_Wtime();

		/* Receive results from worker tasks */
		mtype = FROM_WORKER;
		for (i=1; i<=numworkers; i++)
		{
			source = i;
			MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
			MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
			MPI_Recv(&c[offset][0], rows*NCB, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
		}
		end = MPI_Wtime();
		MPI_Gather(&t, 1, MPI_DOUBLE, &timebuf, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
		best = 10e99;
		worst = 0.0;
		avg = 0.0;
		for (i = 1; i < numtasks; i++)
		{
			if (timebuf[i]>worst)
			{
				worst = timebuf[i];
			}
			if (timebuf[i]<best)
			{
				best = timebuf[i];
			}
			avg = avg + timebuf[i];
		}
		avg = avg / numworkers;
		
		printf("%04d %f %f %f %f %f\n", numtasks, end-start, sendEnd-sendStart, best, avg, worst);
	}


	/**************************** worker task ************************************/
	if (taskid > MASTER)
	{
		mtype = FROM_MASTER;
		MPI_Bcast(&b, NCA*NCB, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

		MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&a, rows*NCA, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
		// MPI_Recv(&b, NCA*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
		start = MPI_Wtime();
		for (k=0; k<NCB; k++)
		{
			for (i=0; i<rows; i++)
			{
				c[i][k] = 0.0;
				for (j=0; j<NCA; j++) {
					c[i][k] = c[i][k] + a[i][j] * b[j][k];
				}
			}
		}
		end = MPI_Wtime();
		mtype = FROM_WORKER;
		MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
		MPI_Send(&c, rows*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
		t = end - start;
		MPI_Gather(&t, 1, MPI_DOUBLE, &timebuf, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}