
for j in 1 2 3 4
do
    mpirun -np 4 working_mpi.c
    for k in 1 2 4 8 16
    do
        sbatch --ntasks-per-node=$k --constraint=moles --mem-per-cpu=1G ./mpi_batch.sh $k
    done
done