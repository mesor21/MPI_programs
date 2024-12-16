#!/bin/bash
#SBATCH --job-name=Menzulskii
#SBATCH --nodes=3
#SBATCH --sockets-per-node=1
#SBATCH --cores-per-socket=16
echo  : >>~/13.out
echo  =$1 >>~/13.out
echo  =$2 >>~/13.out
mpiexec -n $2 ./prog13 $1 >> ~/13.out