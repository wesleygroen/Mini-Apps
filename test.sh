#!/bin/bash
out=strong.txt
# let i=0
# while [ $i -lt 12 ]; do
#     let i+=4
#     prun -np 1 -$i -script \$PRUN_ETC/prun-openmpi ./mpi_pi_reduce | tee -a ${out} &
# done
let i=0
# while [ $i -lt 8 ]; do
#     let i+=1
#     prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ./mpi_pi_reduce | tee -a ${out} &
# done
# while [ $i -lt 16 ]; do
#     let i+=2
#     prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ./mpi_pi_reduce | tee -a ${out} &
# done
# let i=44
# while [ $i -lt 48 ]; do
#     let i+=4
#     prun -np $i -16 -t 40 -script \$PRUN_ETC/prun-openmpi ./mpi_pi_reduce | tee -a ${out} &
# done

# let i=0
# while [ $i -lt 15 ]; do
#     let i+=1
#     prun -np 1 -$i -script \$PRUN_ETC/prun-openmpi ./mpi_pi_weak | tee -a ${out} &
# done
out=weakMPICH.txt

let i=0
while [ $i -lt 20 ]; do
    let i+=1
    prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ./mpi_pi_weak | tee -a ${out} &
done

# prun -np 7 -16 -script \$PRUN_ETC/prun-openmpi ./mpi_pi_weak | tee -a out3.txt &
# out=mm4k.txt
# out=mm2k.txt
# let i=0
# while [ $i -lt 16 ]; do
#     let i+=1
#     prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ./mpi_mm | tee -a ${out} &
# done
# let i=0
# while [ $i -lt 15 ]; do
#     let i+=1
#     prun -np 1 -$i -script \$PRUN_ETC/prun-openmpi ./mpi_pi_weak | tee -a outWeak.txt &
# done
# let i=0
# while [ $i -lt 16 ]; do
#     let i+=1
#     prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ./mpi_pi_weak | tee -a outWeak.txt &
# done