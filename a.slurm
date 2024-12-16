#!/bin/bash
#SBATCH --job-name=Yudina
#SBATCH --nodes=3
#SBATCH --sockets-per-node=1
#SBATCH --cores-per-socket=16
echo  : >>~/26.out
echo  =$1 >>~/26.out
echo  =$2 >>~/26.out
mpiexec -n $2 ./prog26 $1 >> ~/26.out