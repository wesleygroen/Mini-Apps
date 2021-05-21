#!/bin/bash
if [ $# -ne 6 ]; then
    echo 6 Arguments needed
    echo ./test.sh program min max coresPerNode timeLimit output
    exit -1
fi

prog=$1
let start=$2
let max=$3
let cpn=$4
let t=$5
out=$6

let i=${start}
while [ $i -lt ${max} ]; do
    let i+=1
    # echo prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ${prog} \| tee -a ${out} &
    prun -np $i -${cpn} -t $t -script \$PRUN_ETC/prun-openmpi ${prog} >> ${out} &
    # echo prun -np $i -${cpn} -t $t -script \$PRUN_ETC/prun-openmpi ${prog} \>\> ${out} &
    # prun -np $i -${cpn} -t $t -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
done
