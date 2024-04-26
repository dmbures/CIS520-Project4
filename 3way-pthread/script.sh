gcc -o pthreads -pthread -lm working_pthread.c
for j in 1 2 3
do
    for k in 1 2 4 8 16
    do
        for i in 1 2 4 8 16
        do
            sbatch --ntasks-per-node=$k --constraint=moles --mem-per-cpu=1G ./pthread_batch.sh $i $k
        done
    done
done