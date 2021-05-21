#!/bin/bash
if [ $# -ne 5 ]; then
    echo 4 Arguments needed
    echo ./test.sh program min max timeLimit output
    exit -1
fi

prog=$1
let start=$2
let max=$3
let t=$4
out=$5

let i=${start}
while [ $i -lt ${max} ]; do
    let i+=1
    # echo prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ${prog} \| tee -a ${out} &
    prun -np $i -16 -t $t -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
done
