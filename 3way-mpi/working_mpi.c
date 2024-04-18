#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include "sys/types.h"
// #include "sys/sysinfo.h"

#define NUM_THREADS 5
#define NUM_LINES 20
#define LINE_LENGTH 10

char mylines[NUM_LINES][LINE_LENGTH];
int max_per_line[NUM_LINES];
pthread_mutex_t mutexsum;

void read_file(){
    FILE* fp;
    fp = fopen("../test.txt", "r");

    if(!fp){
        perror("Error opening file\n");
        exit(-1);
    }

    char temp_mylines[NUM_LINES][LINE_LENGTH];
    int counter = 0;
    while(counter < NUM_LINES && fgets(temp_mylines[counter],LINE_LENGTH,fp) != NULL){
        memcpy(mylines[counter],temp_mylines[counter],LINE_LENGTH);
        counter++;
    }

    fclose(fp);
}

void* find_max(void* input){
    int section_num = *((int*)input);    
    
    int starting_index = (section_num - 1) * (NUM_LINES / NUM_THREADS);
    int ending_index = starting_index + (NUM_LINES / NUM_THREADS);

    if(ending_index == (NUM_LINES - (NUM_LINES % NUM_THREADS))){
        ending_index = NUM_LINES;
    }

    int local_max[ending_index - starting_index];

    pthread_mutex_lock (&mutexsum);

    for(int i = starting_index; i < ending_index; i++){
        int max = mylines[i][0];
        int j = 0;
        while(j < LINE_LENGTH && mylines[i][j] != '\0'){
            if((int)mylines[i][j] > max){
                max = (int)mylines[i][j];
            }
            local_max[i - starting_index] = max;
            j++;
        }
    }
    
    for (int i = 0; i < ending_index - starting_index; i++)
    {
        max_per_line[i + starting_index] = local_max[i];
    }
    pthread_mutex_unlock (&mutexsum);

    pthread_exit(NULL);
}

int main(void){
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;
    int rc;

    struct timeval t1, t2, t3, t4;
    double elapsedTime;

    
    gettimeofday(&t1, NULL);
    read_file();
    gettimeofday(&t2, NULL);

    rc = MPI_Init(&argc,&argv);

    if (rc != MPI_SUCCESS) {
	    printf ("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
	}

    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    MPI_Bcast(entries, NUM_ENTRIES * LINE_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
    for (int j = 0; j < NUM_THREADS; j++)
    {
        rc = pthread_join(threads[j], &status);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }
    gettimeofday(&t4, NULL);
    MPI_Reduce(local_results_array, results_array, NUM_ENTRIES * LINE_LENGTH, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);

    FILE* data;
    data = fopen("data.txt", "w");

    if(!data){
        perror("Error opening file\n");
        exit(-1);
    }

    for(int i = 0; i < NUM_LINES; i++){
        fprintf(data, "Line %d: %d\n", i, max_per_line[i]);
    }
    
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	printf("Time to read file: %f\n", elapsedTime);
    fprintf(data, "Time to read file: %f\n", elapsedTime);

	elapsedTime = (t4.tv_sec - t3.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t4.tv_usec - t3.tv_usec) / 1000.0; // us to ms
	printf("Time to get max ASCII: %f\n", elapsedTime);
    fprintf(data, "Time to get max ASCII: %f\n", elapsedTime);

    fclose(data);

    pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
}