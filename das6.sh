#!/bin/bash
mkdir -p results
path=results/
let maxNodes=32
let coresPerNode=28
let threadsPerNode=56
let longLimit=600
let midLimit=240
let shortLimit=60
let runs=3

let i=0
while [ $i -lt ${runs} ]; do
    let i+=1
    ./testSlurm.sh 
    ./testSlurm.sh ./mpi_pi_weak ${maxNodes} DAS6_pi_weak${i}
    ./testSlurm.sh ./histogram ${maxNodes} DAS6_histogram${i}
    ./testSlurm.sh ./rgb2gray ${maxNodes} DAS6_rgb2gray${i}
    ./testSlurm.sh ./rgb2yuv ${maxNodes} DAS6_rgb2yuv${i}
    ./testSlurm.sh ./mpi_mm ${maxNodes} DAS6_matrix${i}
    ./testSlurm.sh ./mpi_mm_broadcast ${maxNodes} DAS6_matrix_broadcast${i}
    ./testSlurm.sh ./mpi_pi_strong ${maxNodes} DAS6_pi_strong${i}
    ./testSlurmBW.sh ./mpi_bandwidth ${threadsPerNode} DAS6_pi_bandwidth${i}

    # ./test.sh ./mpi_pi_weak 0 ${maxNodes} ${coresPerNode} ${shortLimit} ${path}weakPi${i}D6.txt
    # ./test.sh ./histogram 0 ${maxNodes} ${coresPerNode} ${midLimit} ${path}histogram${i}D6.txt
    # ./test.sh ./rgb2gray 0 ${maxNodes} ${coresPerNode} ${midLimit} ${path}rgb2gray${i}D6.txt
    # ./test.sh ./rgb2yuv 0 ${maxNodes} ${coresPerNode} ${midLimit} ${path}rgb2yuv${i}D6.txt
    # ./test.sh ./mpi_mm 0 ${maxNodes} ${coresPerNode} ${midLimit} ${path}matrixMultiply${i}D6.txt
    # ./test.sh ./mpi_mm_broadcast 0 ${maxNodes} ${coresPerNode} ${midLimit} ${path}matrixMultiplyBC${i}D6.txt
    # ./test.sh ./mpi_pi_strong 0 4 ${coresPerNode} ${longLimit} ${path}strongPi${i}D6.txt
    # ./test.sh ./mpi_pi_strong 4 8 ${coresPerNode} ${midLimit} ${path}strongPi${i}D6.txt
    # ./test.sh ./mpi_pi_strong 8 ${maxNodes} ${coresPerNode} ${shortLimit} ${path}strongPi${i}D6.txt
    # ./testBW.sh ./mpi_bandwidth 1 ${threadsPerNode} ${path}bandwidth1Node${i}D6.txt
    # ./testBW.sh ./mpi_bandwidth ${maxNodes} 1 ${path}bandwidth1Core${i}D6.txt
    # ./testBW.sh ./mpi_bandwidth 2 ${threadsPerNode} ${path}bandwidth2Nodes${i}D6.txt
    # ./testBW.sh ./mpi_bandwidth 4 ${threadsPerNode} ${path}bandwidth4Nodes${i}D6.txt
    # ./testBW.sh ./mpi_bandwidth 8 ${threadsPerNode} ${path}bandwidth8Nodes${i}D6.txt
    # # ./testBW.sh ./mpi_bandwidth 16 ${threadsPerNode} ${path}bandwidth16Nodes${i}D6.txt
    # # ./testBW.sh ./mpi_bandwidth ${maxNodes} ${threadsPerNode} ${path}bandwidthMaxNodes${i}D6.txt
done
-DNDEBUG -O2 -m64 -march=k8 -fomit-frame-pointer
                          -finline-functions -fno-strict-aliasing -pthread

