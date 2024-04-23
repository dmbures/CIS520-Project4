#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/* Constants */
#define NUM_ENTRIES 1000000 // Should be 1000000
#define LINE_LENGTH 2003 //should be 2003, increasing to this size causes a segmentation fault due to size of table.

int NUM_THREADS;



/* Global Variables */
char entries[NUM_LINES][LINE_LENGTH];
int max_per_line[NUM_LINES];

/* Function prototypes */
void max_ascii_value();
void read_file();

void main() {
	struct timeval t1, t2, t3, t4;
	double elapsedTime;
	int numSlots, myVersion = 2; // 1 = base, 2 = openmp, 3 = pthreads, 4 = mpi

	gettimeofday(&t1, NULL);
        /* Read the file into the list of entries */
    read_file();
    
	gettimeofday(&t2, NULL);
	
    omp_set_num_threads(NUM_THREADS);

	gettimeofday(&t3, NULL);
	/* Get the max substring of each line */
	
    max_ascii_value();
	gettimeofday(&t3, NULL);

	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	printf("Time to read file: %f\n", elapsedTime);

	elapsedTime = (t4.tv_sec - t3.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t4.tv_usec - t3.tv_usec) / 1000.0; // us to ms
	printf("Time to get max substrings: %f\n", elapsedTime);

	printf("DATA, %d, %s, %f\n", myVersion, getenv("SLURM_CPUS_ON_NODE"),  elapsedTime);
    fclose(data);
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
    while (fgets(str1, LINE_LENGTH, fp) != NULL && i < NUM_ENTRIES) {
        strcpy(entries[i], str1);
        i++;
    }

    fclose(fp);
}

/* Find and print the maximum ASCII value of each line */
void max_ascii_value() {
    int max_val;
    #pragma omp parallel for private(max_val)
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