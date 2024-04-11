#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include "sys/types.h"
// #include "sys/sysinfo.h"

#define NUM_THREADS 20
#define CHUNK_SIZE 6000
#define TOTAL_SIZE 1000000
#define LINE_LENGTH 10000

pthread_mutex_t mutexsum;
int global_index = 0;

char mylines[CHUNK_SIZE][LINE_LENGTH];
int max_per_line[CHUNK_SIZE];

typedef struct {
	uint32_t virtualMem;
	uint32_t physicalMem;
} processMem_t;

struct args{
    int id;
    int index;
};

int parseLine(char *line) {
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9') p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
}

void GetProcessMemory(processMem_t* processMem) {
	FILE *file = fopen("/proc/self/status", "r");
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmSize:", 7) == 0) {
			processMem->virtualMem = parseLine(line);
		}

		if (strncmp(line, "VmRSS:", 6) == 0) {
			processMem->physicalMem = parseLine(line);
		}
	}
	fclose(file);
}

void read_chunk_of_lines(int index, FILE *fd)
{
    char local_mylines[CHUNK_SIZE][LINE_LENGTH];
    
    int i, upper_bound;
    
    for (i = 0; i < CHUNK_SIZE ; i++)
    {
        fgets(local_mylines[i],LINE_LENGTH,fd);
    }

    if(index * CHUNK_SIZE + CHUNK_SIZE  > TOTAL_SIZE)
    {
        upper_bound = TOTAL_SIZE - index * CHUNK_SIZE;
    }
    else
    {
        upper_bound =CHUNK_SIZE;
    }

    for (i = 0; i < upper_bound; i++)
    {
        memcpy(mylines[i],local_mylines[i],LINE_LENGTH);
    }  
    
}

void* find_max(void* input) {
    int index = global_index;
    int id = *((int*)input);
    int i,j, upper_bound;
    int max[CHUNK_SIZE];

    if(index * CHUNK_SIZE + id*(CHUNK_SIZE/NUM_THREADS) + (CHUNK_SIZE/NUM_THREADS)  > TOTAL_SIZE)
    {
        upper_bound = TOTAL_SIZE - (index * CHUNK_SIZE + id*(CHUNK_SIZE/NUM_THREADS));
    }
    else
    {
        upper_bound = id * CHUNK_SIZE/NUM_THREADS + CHUNK_SIZE/NUM_THREADS ;
    }
    
    for (i = id*(CHUNK_SIZE/NUM_THREADS); i < id*(CHUNK_SIZE/NUM_THREADS) +(CHUNK_SIZE/NUM_THREADS) ; i++)
    {
        max[i] = 0;
        for(j = 0;j < LINE_LENGTH;j++)
        {
            if((int)(mylines[i][j]) > max[i])
            {
                max[i] = (int)(mylines[i][j]);
            }
        }
            
    }
    
        
    pthread_mutex_lock (&mutexsum);
    for (i =  id*(CHUNK_SIZE/NUM_THREADS); i < upper_bound; i++)
    {
        max_per_line[i] = max[i];
    }
    pthread_mutex_unlock (&mutexsum);

    pthread_exit(NULL);   
    return NULL;
}

void print_results(int index)
{
    int upper_bound;
    if(index * CHUNK_SIZE + CHUNK_SIZE  > TOTAL_SIZE)
    {
        upper_bound = TOTAL_SIZE - (index * CHUNK_SIZE);
    }
    else
    {
        upper_bound = CHUNK_SIZE;
    }
    for (int i = 0; i < upper_bound; i++)
    {
        printf("%d: %d\n",i + (index * CHUNK_SIZE),max_per_line[i + (index * CHUNK_SIZE)]);
    }
}

int main(void)
{
    printf("Starting");
    FILE *fd;
    //fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    fd = fopen("./test.txt", "r");
    printf("Made it here");
    int i, rc;
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int index = 0;
    while ((index * CHUNK_SIZE) < TOTAL_SIZE)
    {
        read_chunk_of_lines(index, fd);

        for (i = 0; i < NUM_THREADS; i++)
        {
            rc = pthread_create(&threads[i], &attr, find_max, (void*)&i);
            if (rc)
            {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                fclose(fd);
                exit(-1);
            }
        }
        /* Free attribute and wait for the other threads */
        pthread_attr_destroy(&attr);
        for (int j = 0; j < NUM_THREADS; j++)
        {
            rc = pthread_join(threads[j], &status);
            if (rc)
            {
                printf("ERROR; return code from pthread_join() is %d\n", rc);
                fclose(fd);
                exit(-1);
            }
        }
        print_results(index);

        index++;
        global_index++;
    }

    for (i = 0; i < TOTAL_SIZE; i++)
    {
        printf(" %d : %d \n",i, max_per_line[i]);
    }
    // Print memory ussage
    processMem_t myMem;
    GetProcessMemory(&myMem);
    printf("Memory: vMem %u KB, pMem %u KB\n", myMem.virtualMem, myMem.physicalMem);


    pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
    fclose(fd);
}
