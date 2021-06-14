#!/usr/bin/env python
import sys
nodes = '#SBATCH -N '
tasks = '#SBATCH --ntasks-per-node='
outfile = '#SBATCH --output=0%j_'+sys.argv[1]
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
c = 1
for n in [1,2,4]:
    lines[4]=outfile+str(n)+'\n'
    # print(lines[4])
    lines[2]=nodes+str(n)+"\n"
    for i in range(2,int(sys.argv[2])+1, 2):
        # print(n, i)
        lines[3]=tasks+str(i)+'\n'
        f = open(str(c)+".slurmm", 'w')
        f.writelines(lines)
        f.close()
        c+=1
