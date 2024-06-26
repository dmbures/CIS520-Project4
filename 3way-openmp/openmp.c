#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/* Constants */
#define NUM_ENTRIES 1000000 // Should be 1000000
#define NUM_THREADS 8
#define LINE_LENGTH 2003 //should be 2003, increasing to this size causes a segmentation fualt due to size of table.

/* Global Variables */
char entries[NUM_ENTRIES][LINE_LENGTH];

/* Function prototypes */
void max_substring(int myID);
char *strrev(char *str);
void read_file();

void main() {
	struct timeval t1, t2, t3, t4;
	double elapsedTime;
	int numSlots, myVersion = 2; // 1 = base, 2 = openmp, 3 = pthreads, 4 = mpi

	gettimeofday(&t1, NULL);
	/* Read the file into the the list of entries */
	read_file();
	gettimeofday(&t2, NULL);

	omp_set_num_threads(NUM_THREADS);
	
	gettimeofday(&t3, NULL);
	/* Get the max substring of each line */
	#pragma omp parallel
	{
		max_substring(omp_get_thread_num());
	}
	gettimeofday(&t4, NULL);

	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	printf("Time to read file: %f\n", elapsedTime);

	elapsedTime = (t4.tv_sec - t3.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t4.tv_usec - t3.tv_usec) / 1000.0; // us to ms
	printf("Time to get max substrings: %f\n", elapsedTime);

	printf("DATA, %d, %s, %f\n", myVersion, getenv("SLURM_CPUS_ON_NODE"),  elapsedTime);
}

/* Read the file from wiki_dump.txt into the list of entries */
void read_file() {
	FILE *fp;
	char str1[LINE_LENGTH];
	fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
	// fp = fopen("test.txt", "r");

	/* If the file could not be found, return */
	if(fp == NULL) {
		perror("Failed: ");
		return;
	}
	
	/* Add each line of the file into entries */
	int i = 0;
	while(fgets(str1, LINE_LENGTH, fp) != NULL && i < NUM_ENTRIES){
		strcpy(entries[i], str1);
		i++;
	}

	fclose(fp);
}

/* Find and prints the biggest AND most common substring between 2 str1 and str2.
   Code inspired by https://www.geeksforgeeks.org/longest-common-substring/ */
void max_substring(int myID) {
	int startPos = myID * (NUM_ENTRIES / NUM_THREADS);
	int endPos = startPos + (NUM_ENTRIES / NUM_THREADS);
	
	char str1[LINE_LENGTH];
	char str2[LINE_LENGTH];
	int m, n, i;
	char biggest[LINE_LENGTH]; // The biggest/most common substring
	char temp[LINE_LENGTH];
	
	int row, col;
	int biggest_row, biggest_col; // Index of the LAST letter of the biggest substring we've found
	int max_len; // The length of the biggest substring we've found

	#pragma omp private(str1,str2,startPos,endPos,m,n,i,biggest,temp,row,col,biggest_row,biggest_col) 
		for(i = startPos; i < endPos; i++)
		{
			strcpy(str1, entries[i]);
			strcpy(str2, entries[i+1]);
			m = strlen(str1);
			n = strlen(str2);

			/* Allocate memory for a table */
			int (*table)[n] = malloc(sizeof(int[m + 1][n + 1]));
			
			max_len = 0;
			row =  0; 
			col = 0;
			biggest_row = 0;
			biggest_col = 0;
			
			/* Populate the table */
			for(row = 0; row < m; row++) {
				for(col = 0; col < n; col++) {
					/* Initialize table[1..m][0] and table[0][1..n] to 1 */
					if(row == 0 || col == 0) {
						table[row][col] = 1;
					}
					/* If the letters of the two strings are the same, add 1 to the left diagonal  
					   value and set it to the current position */
					else if(str1[row] == str2[col] && str1[row] != '\n'){
						table[row][col] = table[row-1][col-1] + 1;
						
						/* If the current position is bigger than the biggest substring
						   we've found so far, update our variables so that we can find
						   it later on. */
						if(table[row][col] > max_len) {
							max_len = table[row][col];
							biggest_row = row;
							biggest_col = col;
						}
					}
					/* If the letters are not the same, the substring length is 0 */
					else {
						table[row][col] = 0;				
					}
				}
			}

			if(max_len == 0) {
				printf("%d-%d:, %s\n", i, i+1, "No common substring.");
			}
			else{
				memset(biggest, '\0', LINE_LENGTH);
				/* Starting at the bottom right of the biggest substring in the table, build biggest substring */
				while(table[biggest_row][biggest_col] != 1) {
				 	/* Convert the letter to a string, since strcat requires a string */				
					memset(temp, '\0', LINE_LENGTH);
					
					temp[0] = str1[biggest_row];
	
				 	/* Concatonate the string */
					strcat(biggest, temp);
					
				 	/* Move to the next letter (top left) in the backwards substring */
					biggest_row--;
					biggest_col--;
				}
				strcat(biggest, "\0");

				/* Print and reverse the biggest substring */
				printf("%d-%d: %s\n", i, i+1, strrev(biggest));
			}

			/* Free the table */
			free(table);
		}
} 

/* Reverse a string
   From https://stackoverflow.com/questions/8534274/is-the-strrev-function-not-available-in-linux */
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}