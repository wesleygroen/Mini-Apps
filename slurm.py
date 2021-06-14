#!/usr/bin/env python
nodes = '#SBATCH -N '
import sys
lines = [ '#!/bin/bash\n',
'#SBATCH --time=00:15:00\n',
'#SBATCH -N x\n',
'#SBATCH --ntasks-per-node=24\n',
'#SBATCH --output=0%j_'+sys.argv[1]+'.out\n',
# '#SBATCH --output='+sys.argv[1]+'_%j.out\n',
# '# #SBATCH --error=error_%j.out\n',
'#SBATCH --mail-type=END\n',
'#SBATCH --mail-user=wesleygroen.wg@gmail.com\n',
'APP=$1\n',
'ARGS=""\n',
'OMPI_OPTS="--mca btl ^usnic"\n',
'MPI_RUN=mpirun\n',
'$MPI_RUN $OMPI_OPTS $APP $ARGS\n'
]
# f = open(sys.argv[1], 'r')
# lines = f.readlines()
# f.close()
for i in range(1,int(sys.argv[2])+1):
    f = open(str(i)+".slurmm", 'w')
    lines[2]=nodes+str(i)+"\n"
    f.writelines(lines)
    f.close()
