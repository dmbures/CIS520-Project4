#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>

#define NUM_LINES 1000000
#define LINE_LENGTH 1000
int NUM_THREADS;

char entries[NUM_LINES][LINE_LENGTH];

int max_per_line[NUM_LINES];
int local_results_array[NUM_LINES];

/* Function prototypes */
void max_ascii_value(void *rank);
void read_file();

int main(int argc, char* argv[]) {
    struct timeval t1, t2, t3, t4;
    double elapsedTime;
    //int myVersion = 4; // 1 = base, 2 = openmp, 3 = pthreads, 4 = mpi

    int rc;
    int numtasks, rank;

    FILE* data;
    data = fopen("data.txt", "w");

    rc = MPI_Init(&argc,&argv);
    if (rc != MPI_SUCCESS) {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    NUM_THREADS = numtasks;
    //printf("size = %d rank = %d\n", numtasks, rank);
    fflush(stdout);

    gettimeofday(&t1, NULL);
    if (rank == 0) {
        /* Read the file into the list of entries */
        read_file();
    }
    gettimeofday(&t2, NULL);

    MPI_Bcast(entries, NUM_LINES * LINE_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
    gettimeofday(&t3, NULL);
    /* Get the maximum ASCII value of each line */
    max_ascii_value(&rank);
    gettimeofday(&t4, NULL);
    MPI_Reduce(local_results_array, max_per_line, NUM_LINES, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    /*
    for(int i = 0; i < NUM_LINES; i++){
        fprintf(data, "Line %d: %d\n", i, max_per_line[i]);
    }
    */

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
    double readFileTime = elapsedTime;
    printf("Time to read file: %f\n", elapsedTime);
    //fprintf(data, "Time to read file: %f\n", elapsedTime);

    elapsedTime = (t4.tv_sec - t3.tv_sec) * 1000.0; //sec to ms
    elapsedTime += (t4.tv_usec - t3.tv_usec) / 1000.0; // us to ms
    double maxASCIITime = elapsedTime;
    printf("Time to get maximum ASCII values: %f\n", elapsedTime);
    //fprintf(data, "Time to get max ASCII: %f\n", elapsedTime);

    //printf("DATA, %d, %s, %f\n", myVersion, getenv("SLURM_CPUS_ON_NODE"), elapsedTime);
    printf("DATA: %s, %f, %f", getenv("SLURM_CPUS_ON_NODE"), readFileTime, maxASCIITime);
    fclose(data);

    MPI_Finalize();
    return 0;
}

/* Read the file from wiki_dump.txt into the list of entries */
void read_file() {
    FILE *fp;
    char str1[LINE_LENGTH];
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");

    /* If the file could not be found, return */
    if (fp == NULL) {
        perror("Failed: ");
        return;
    }

    /* Add each line of the file into entries */
    int i = 0;
    while (fgets(str1, LINE_LENGTH, fp) != NULL && i < NUM_LINES) {
        strcpy(entries[i], str1);
        i++;
    }

    fclose(fp);
}

/* Find and print the maximum ASCII value of each line */
void max_ascii_value(void *rank) {
    int myID = *((int *) rank);

    int startPos = myID * (NUM_LINES / NUM_THREADS);
    int endPos = startPos + (NUM_LINES / NUM_THREADS);

    char str[LINE_LENGTH];
    int i, j, max_val;

    for (i = startPos; i < endPos; i++) {
        strcpy(str, entries[i]);
        int len = strlen(str);
        max_val = 0;

        for (j = 0; j < len; j++) {
            if ((int)str[j] > max_val) {
                max_val = (int)str[j];
            }
        }

        // printf("%d: %d\n", i, max_val); // HERE IT WILL PRINT EVERY SINGLE VALUE
        // printf("line: %s\n", entries[i]);
        // printf("Press Enter to continue...");
        // getchar(); // Wait for user to press Enter
        local_results_array[i] = max_val;
    }
}
