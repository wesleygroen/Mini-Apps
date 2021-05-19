#!/bin/bash
if [ $# -ne 4 ]; then
    echo 4 Arguments needed
    echo ./test.sh program min max output
    exit -1
fi

prog=$1
let start=$2
let max=$3
out=$4

let i=${start}
while [ $i -lt ${max} ]; do
    let i+=1
    # echo prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ${prog} \| tee -a ${out} &
    prun -np $i -16 -t 60 -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
done
