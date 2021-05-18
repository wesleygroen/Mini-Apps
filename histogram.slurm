#!/bin/bash

##SBATCH --nodes=20
#SBATCH --job-name=hist
#SBATCH --partition=defq
#SBATCH --array=1-5
#SBATCH --tasks-per-node=16


# Additional directives can control how
# tasks should be distributed to individual nodes
# #SBATCH --time=00:05:00
# #SBATCH --tasks=48
# #SBATCH --cpus-per-task=1

echo "Starting calculation at $(date)"
out=hist5.txt
# let i=0
# while [ $i -lt 20 ]; do
#     let i+=1
    # prun -np $i -16 -t 120 -script \$PRUN_ETC/prun-openmpi ./histogram | tee -a ${out} &
mpiexec -n $SLURM_ARRAY_TASK_ID ./histogram | tee -a ${out} &
# done
echo "Completed calculation at $(date)"