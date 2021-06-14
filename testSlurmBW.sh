#!/bin/bash
if [ $# -ne 3 ]; then
    echo 3 Arguments needed
    echo ./test.sh program maxNodes output
    exit -1
fi

prog=$1
# let start=$2
let max=$2
# let cpn=$4
# let t=$5
out=$3

# let i=${start}
let i=0
./slurmBW.py $out $max
while [ $i -lt ${max} ]; do
    let i+=1
    sbatch ${i}.slurmm ${prog}
    # echo prun -np $i -16 -script \$PRUN_ETC/prun-openmpi ${prog} \| tee -a ${out} &
    # prun -np $i -${cpn} -t $t -script \$PRUN_ETC/prun-openmpi ${prog} >> ${out} &
    # prun -np $i -${cpn} -t $t -script ./prun-openmpi.sh ${prog} >> ${out} &
    # echo prun -np $i -${cpn} -t $t -script \$PRUN_ETC/prun-openmpi ${prog} \>\> ${out} &
    # prun -np $i -${cpn} -t $t -script \$PRUN_ETC/prun-openmpi ${prog} | tee -a ${out} &
done
rm -f *.slurmm
