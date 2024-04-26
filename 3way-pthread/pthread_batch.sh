#!/bin/bash
#SBATCH -J job0
#SBATCH --mail-type=END
#SBATCH --mail-user change@ksu.edu
#SBATCH --constraint=moles
cd $HOME/hw4/CIS520-Project4/3way-pthread/build
./working_pthread_test $1 $2