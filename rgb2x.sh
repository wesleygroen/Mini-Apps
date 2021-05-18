#!/bin/bash
out=rgb2gray2.txt
let i=0
while [ $i -lt 20 ]; do
    let i+=1
    prun -np $i -16 -t 300 -script \$PRUN_ETC/prun-openmpi ./rgb2gray | tee -a ${out} &
done
# out=rgb2yuv.txt
# let i=0
# while [ $i -lt 20 ]; do
#     let i+=1
#     prun -np $i -16 -t 300 -script \$PRUN_ETC/prun-openmpi ./rgb2yuv | tee -a ${out} &
# done
# while [ $i -lt 48 ]; do
#     let i+=1
#     prun -np $i -16 -t 40 -script \$PRUN_ETC/prun-openmpi ./histogram | tee -a ${out} &
# done