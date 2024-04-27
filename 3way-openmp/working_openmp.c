#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

/* Constants */
#define NUM_LINES 1000000 // Should be 1000000
#define LINE_LENGTH 1000 //should be 2003, increasing to this size causes a segmentation fault due to size of table.

int numThreads;

/* Global Variables */
char mylines[NUM_LINES][LINE_LENGTH];
int max_per_line[NUM_LINES];

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
        strcpy(mylines[i], str1);
        i++;
    }

    fclose(fp);
}

/*
void max_ascii_value() {
    int max_val;

    for (int i = 0; i < NUM_LINES; i++) {
        char *str = entries[i];
        int len = strlen(str);
        max_val = 0;

        for (int j = 0; j < len; j++) {
            if ((int)str[j] > max_val) {
                max_val = (int)str[j];
            }
        }
        // printf("%d: %d\n", i, max_val); // HERE IT WILL PRINT EVERY SINGLE VALUE
        // printf("line: %s\n", entries[i]);
        // printf("Press Enter to continue...");
        // getchar(); // Wait for user to press Enter
        max_per_line[i] = max_val;
    }
}
*/

void find_max(void* input){
    int section_num = *((int*)input);    
    
    int starting_index = (section_num - 1) * (NUM_LINES / numThreads);
    int ending_index = starting_index + (NUM_LINES / numThreads);

    if(ending_index == (NUM_LINES - (NUM_LINES % numThreads))){
        ending_index = NUM_LINES;
    }

    int local_max[ending_index - starting_index];

    #pragma omp private(starting_index, ending_index, max_per_line, mylines, local_max);

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
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        return EXIT_FAILURE;
    }
    struct timeval t1, t2, t3, t4;
	double elapsedTime;
	//int myVersion = 2; // 1 = base, 2 = openmp, 3 = pthreads, 4 = mpi

	gettimeofday(&t1, NULL);
        /* Read the file into the list of entries */
    read_file();
    
	gettimeofday(&t2, NULL);
	
    omp_set_num_threads(numThreads);

	gettimeofday(&t3, NULL);
	/* Get the max substring of each line */
	
    #pragma omp parallel find_max(omp_get_thread_num());

	gettimeofday(&t4, NULL);

	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
    double readFileTime = elapsedTime;
	printf("Time to read file: %f\n", elapsedTime);

	elapsedTime = (t4.tv_sec - t3.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t4.tv_usec - t3.tv_usec) / 1000.0; // us to ms
    double maxASCIITime = elapsedTime;
	printf("Time to get max substrings: %f\n", elapsedTime);

	printf("DATA: %d, %d, %f, %f", numThreads, atoi(argv[2]), readFileTime, maxASCIITime);

    return 0;
}