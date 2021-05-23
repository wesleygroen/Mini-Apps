#!/bin/bash
if [ $# -ne 4 ]; then
    echo 4 Arguments needed
    echo ./test.sh program maxNodes maxCoresPerNode output
    exit -1
fi

prog=$1
let nodes=$2
let cpn=$3
out=$4

let i=0
if [ $nodes -eq 1 ]; 
then
    while [ $i -lt ${cpn} ]; do
        let i+=2
        # echo prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ${prog} \| tee -a ${out} &
        prun -np 1 -$i -t 600 -script \$PRUN_ETC/prun-openmpi ${prog} >> ${out} &
        # echo prun -np 1 -$i -t 300 -script \$PRUN_ETC/prun-openmpi ${prog} \>\> ${out} &
        # echo prun -np 1 -$i -t 300 -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
    done
else 
    if [ $cpn -eq 1 ]; 
    then
        while [ $i -lt ${nodes} ]; do
            let i+=2
            # echo prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ${prog} \| tee -a ${out} &
            prun -np $i -${cpn} -t 600 -script \$PRUN_ETC/prun-openmpi ${prog} >> ${out} &
            # echo prun -np $i -${cpn} -t 300 -script \$PRUN_ETC/prun-openmpi ${prog} \>\> ${out} &
            # echo prun -np $i -${cpn} -t 300 -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
        done
    else
        while [ $i -lt ${cpn} ]; do
            let i+=2
            # echo prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ${prog} \| tee -a ${out} &
            prun -np $nodes -$i -t 600 -script \$PRUN_ETC/prun-openmpi ${prog} >> ${out} &
            # echo prun -np $nodes -$i -t 300 -script \$PRUN_ETC/prun-openmpi ${prog} \>\> ${out} &
            # echo prun -np 1 -$i -t 300 -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
        done
    fi
fi
# prun -np $nodes -$cpn -t 300 -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
