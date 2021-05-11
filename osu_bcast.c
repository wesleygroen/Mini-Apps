#define BENCHMARK "OSU MPI%s Broadcast Latency Test"
/*
 * Copyright (C) 2002-2021 the Network-Based Computing Laboratory
 * (NBCL), The Ohio State University.
 *
 * Contact: Dr. D. K. Panda (panda@cse.ohio-state.edu)
 *
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level OMB directory.
 */
// #include <osu_util_mpi.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define ITERATIONS          100
#define TESTS               10
#define LARGE_MESSAGE_SIZE  8192
#define ENDSIZE             134217728

int main(int argc, char *argv[])
{
    int i = 0, rank, size, end, rndtrps, seed;
    int numprocs, sizes[TESTS];
    double avg_time = 0.0, max_time = 0.0, min_time = 0.0;
    double latency = 0.0, t_start = 0.0, t_stop = 0.0;
    double  sendbuf[ENDSIZE], recvbuf[ENDSIZE], minbw, maxbw, avgbw, msg_size;
    double timer=0.0;
    char *buffer=NULL;
    int po_ret;
    // options.bench = COLLECTIVE;
    // options.subtype = LAT;

    // set_header(HEADER);
    // set_benchmark_name("osu_bcast");
    // po_ret = process_options(argc, argv);

    // if (PO_OKAY == po_ret && NONE != options.accel) {
    //     if (init_accel()) {
    //         fprintf(stderr, "Error initializing device\n");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    // start = STARTSIZE;
    end = ENDSIZE;
    // incr = INCREMENT;
    rndtrps = ITERATIONS;
    seed = 42;
    srand(seed);
    sizes[0] = 1;
    sizes[1] = 1000;
    sizes[2] = 100000;
    sizes[3] = 1;
    sizes[4] = 1000;
    sizes[5] = 100000;
    sizes[6] = 1;
    sizes[7] = 1000;
    sizes[8] = 100000;
    sizes[9] = end;
    // sizes[3] = end;
    for (i=0; i<end; i++){
        sendbuf[i] = (double)rand()/RAND_MAX;
        // msgbuf[i] = 1.79769e+300;
    }

    // switch (po_ret) {
    //     case PO_BAD_USAGE:
    //         print_bad_usage_message(rank);
    //         MPI_Finalize();
    //         exit(EXIT_FAILURE);
    //     case PO_HELP_MESSAGE:
    //         print_help_message(rank);
    //         MPI_Finalize();
    //         exit(EXIT_SUCCESS);
    //     case PO_VERSION_MESSAGE:
    //         print_version_message(rank);
    //         MPI_Finalize();
    //         exit(EXIT_SUCCESS);
    //     case PO_OKAY:
    //         break;
    // }

    if(numprocs < 2) {
        if (rank == 0) {
            fprintf(stderr, "This test requires at least two processes\n");
        }

        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    // if (options.max_message_size > options.max_mem_limit) {
    //     if (rank == 0) {
    //         fprintf(stderr, "Warning! Increase the Max Memory Limit to be able to run up to %ld bytes.\n"
    //                         "Continuing with max message size of %ld bytes\n", 
    //                         options.max_message_size, options.max_mem_limit);
    //     }
    //     options.max_message_size = options.max_mem_limit;
    // }

    // if (allocate_memory_coll((void**)&buffer, options.max_message_size, options.accel)) {
    //     fprintf(stderr, "Could Not Allocate Memory [rank %d]\n", rank);
    //     MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    // }
    // set_buffer(buffer, options.accel, 1, options.max_message_size);

    // print_preamble(rank);
    if (rank==0) {
        printf("\n#=============#=================#=================#=================#\n");
        printf(  "|     size    |       avg       |       min       |       max       |\n");
        printf(  "#=============#=================#=================#=================#\n");
    }
    
    for(int ni = 0; ni < TESTS; ni++) {
        // if(size > LARGE_MESSAGE_SIZE) {
        //     options.skip = options.skip_large; 
        //     options.iterations = options.iterations_large;
        // }
        size = sizes[ni];
        timer=0.0;
        for(i=0; i < ITERATIONS ; i++) {
            t_start = MPI_Wtime();
            MPI_Bcast(&sendbuf, size, MPI_CHAR, 0, MPI_COMM_WORLD);
            t_stop = MPI_Wtime();
            timer+=t_stop-t_start;
            MPI_Barrier(MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        latency = (timer * 1e6) / ITERATIONS;

        MPI_Reduce(&latency, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
        MPI_Reduce(&latency, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        MPI_Reduce(&latency, &avg_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        avg_time = avg_time/numprocs;
        if (rank == 0) {
            // print_stats(rank, size, avg_time, min_time, max_time);
            // msg_size = ((double)sizeof(double)*size);
            // maxbw = min_time/1048576.0;
            // avgbw = (totalbw/1048576.0);
            // minbw = minbw/1048576.0;
            printf("| %*d |", 11, size, avg_time);
            printf(" %*f |", 15, avg_time);
            printf(" %*f |", 15, min_time);
            printf(" %*f |\n", 15, max_time);
        }
    }
    if (rank == 0) {
        printf(  "#=============#=================#=================#=================#\n");
    }
    
    // free_buffer(buffer, options.accel);

    MPI_Finalize();

    // if (NONE != options.accel) {
    //     if (cleanup_accel()) {
    //         fprintf(stderr, "Error cleaning up device\n");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    return EXIT_SUCCESS;
}

/* vi: set sw=4 sts=4 tw=80: */
