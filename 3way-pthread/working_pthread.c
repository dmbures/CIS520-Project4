#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include "sys/types.h"
// #include "sys/sysinfo.h"

#define NUM_THREADS 4
#define NUM_LINES 10
#define LINE_LENGTH 20

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

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    read_file();

    for(int i = 0; i < NUM_THREADS; i++){
        rc = pthread_create(&threads[i], &attr, find_max, (void*)&i);

        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);
    for (int j = 0; j < NUM_THREADS; j++)
    {
        rc = pthread_join(threads[j], &status);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    for(int i = 0; i < NUM_LINES; i++){
        printf("Line %d: %d\n", i, max_per_line[i]);
    }

    pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
}
