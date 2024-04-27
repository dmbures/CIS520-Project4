#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include "sys/types.h"
// #include "sys/sysinfo.h"

#define NUM_LINES 1000000
#define LINE_LENGTH 1000

char mylines[NUM_LINES][LINE_LENGTH];
int max_per_line[NUM_LINES];
pthread_mutex_t mutexsum;
int numThreads = 1;

void read_file(){
    FILE* fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");

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
    
    int starting_index = (section_num - 1) * (NUM_LINES / numThreads);
    int ending_index = starting_index + (NUM_LINES / numThreads);

    if(ending_index == (NUM_LINES - (NUM_LINES % numThreads))){
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



int main(int argc, char *argv[]){
    if(argc < 3){
        return EXIT_FAILURE;
    }

    numThreads = atoi(argv[1]);

    pthread_t threads[numThreads];
    pthread_attr_t attr;
    void *status;
    int rc;

    struct timeval t1, t2, t3, t4;
    double elapsedTime;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    gettimeofday(&t1, NULL);
    read_file();
    gettimeofday(&t2, NULL);

    gettimeofday(&t3, NULL);
    for(int i = 0; i < numThreads; i++){
        rc = pthread_create(&threads[i], &attr, find_max, (void*)&i);

        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);
    for (int j = 0; j < numThreads; j++)
    {
        rc = pthread_join(threads[j], &status);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }
    gettimeofday(&t4, NULL);

    /*
    FILE* data;
    data = fopen("data.txt", "w");

    
    if(!data){
        perror("Error opening file\n");
        exit(-1);
    }
    */
    
    /*
    for(int i = 0; i < NUM_LINES; i++){
        printf("Line %d: %d\n", i, max_per_line[i]);
    }
    */
    
    printf("Number of Threads: %d\n", numThreads);
    printf("Number of Cores: %d\n", atoi(argv[2]));

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
    double readFileTime = elapsedTime;
	printf("Time to read file: %f\n", elapsedTime);
    //fprintf(data, "Time to read file: %f\n", elapsedTime);

	elapsedTime = (t4.tv_sec - t3.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t4.tv_usec - t3.tv_usec) / 1000.0; // us to ms
    double maxASCIITime = elapsedTime;
	printf("Time to get max ASCII: %f\n", elapsedTime);
    //fprintf(data, "Time to get max ASCII: %f\n", elapsedTime);

    //fclose(data);

    printf("DATA: %d, %d, %f, %f", numThreads, atoi(argv[2]), readFileTime, maxASCIITime);
    
    pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
}
