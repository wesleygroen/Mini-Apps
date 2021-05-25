#!/bin/sh

# Sanity checks to make sure we are running under prun/SLURM:
if [ "X$SLURM_JOB_ID" = X ]; then
    echo "No SLURM_JOB_ID in environment; not running under SLURM?" >&2
    exit 1
fi
if [ "X$PRUN_PE_HOSTS" = X ]; then
    echo "No PRUN_PE_HOSTS in environment; not running under prun?" >&2
    exit 1
fi

# module load openmpi/gcc/64

# Construct host file for OpenMPI's mpirun:
mkdir -p $HOME/tmp
NODEFILE=$HOME/tmp/hosts.$SLURM_JOB_ID

# Configure specified number of CPUs per node:
( for i in $PRUN_PE_HOSTS; do
    echo $i slots=$PRUN_CPUS_PER_NODE
  done
) > $NODEFILE

if [ "X$OMPI_OPTS" = X ]; then
    OMPI_OPTS="--mca btl ^usnic"
fi
$MPI_RUN $OMPI_OPTS --hostfile $NODEFILE $PRUN_PROG $PRUN_PROGARGS
retval=$?

rm $NODEFILE
exit $retval