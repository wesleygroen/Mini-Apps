#!/bin/bash
out=hist4.txt
let i=0
# while [ $i -lt 8 ]; do
#     let i+=2
#     prun -np 1 -$i -t 60 -script \$PRUN_ETC/prun-openmpi ./histogram | tee -a ${out} &
# done
# while [ $i -lt 16 ]; do
#     let i+=4
#     prun -np 1 -$i -t 60 -script \$PRUN_ETC/prun-openmpi ./histogram | tee -a ${out} &
# done
# let i=1
while [ $i -lt 52 ]; do
    let i+=1
    prun -np $i -16 -t 120 -script \$PRUN_ETC/prun-openmpi ./histogram | tee -a ${out} &
done
# while [ $i -lt 48 ]; do
#     let i+=1
#     prun -np $i -16 -t 40 -script \$PRUN_ETC/prun-openmpi ./histogram | tee -a ${out} &
# done