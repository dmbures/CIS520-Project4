#!/bin/bash
#SBATCH -J job0
#SBATCH --mail-type=END
#SBATCH --mail-user change@ksu.edu
#SBATCH --constraint=moles
load module openmpi

cd $HOME/hw4/CIS520-Project4/3way-mpi/build
./mpi_test $1