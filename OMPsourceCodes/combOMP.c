/*
OpenMP
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

/* The file form where we will read the specified word */
const char *FILE_NAME = "try.txt";

/* Function that counts the occurences of ones */
long number_of_ones( int, int );

/* The word that we will count the occurences of 1 */
char *msg = NULL;

int main(int argc, char **argv){

	//////////////////////////
	struct timeval tempo1, tempo2;
	
	long elapsed_utime;    /* elapsed time in microseconds */
    long elapsed_mtime;    /* elapsed time in milliseconds */
    long elapsed_seconds;  /* diff between seconds counter */
    long elapsed_useconds; /* diff between microseconds counter */	
	
	//Variable declaration
	int i;
	//int j;
	//int rows_of_proc;
	int send_count = 0;
	long num_of_ones = 0; 
	long result = 0;
	
	/* number of characters in the file */
	int count = 0;	
	int rounds = 0;
	
	/* Number of threads */
	int thread_num = 0;
	
	/* Thread variables */
	int tid;
	int start;
	int end;	
	
	/* Get the number of threads */
	if(atoi(argv[1]) > 0 && atoi(argv[1]) <= 8)
		thread_num = atoi(argv[1]);
	else {
			printf("Wrong arguments, terminating...\n");	
			exit(0);
		}

	/* Set the number of threads */
	omp_set_num_threads( thread_num ) ;
	
		int ch;
		
		/* input file */
		FILE *in_file;
		
		/* open the file if possible, else abort */
		in_file = fopen(FILE_NAME, "r");
		if (in_file == NULL) {
			printf("Cannot open %s\n", FILE_NAME);
			exit(0);
		}
	
		/* Read all the characters of the file */
		while (1) {
			ch = fgetc(in_file);
			/* Check if we are at end of file */
			if (ch == EOF)
				break;
			++count;
		}
	
		printf("Number of characters in %s is %d\n",
		FILE_NAME, count);
		
		/* Close the file */
		fclose(in_file);	
		
		/* open the file if possible, else abort */
		in_file = fopen(FILE_NAME, "r");		
		
		/* The word that we count to count the
		occurences of 1*/
		msg = (char *) malloc( (count ) * sizeof(char) );
		int counter = 0;
		
		/* Read all the characters of the file
		and store them in the word*/
		while (1) {
			ch = fgetc(in_file);
			/* Check if we are at end of file */
			if (ch == EOF)
				break;
			msg[counter++] = (char)ch;
			
		}	
		/* String termination character */	
		msg[counter-1] = '\0';
	
		/* Close the file */
		fclose(in_file);				
		
	/* the total ampunt of work that each thread will perform */	
	int work = strlen(msg);	
	printf("Work is %d\n", work);
	
	/* array that holds the number of '1' that each thread has computed */
	int thread_word[8] = {0};

	/* Start counting the time */	
	gettimeofday(&tempo1, NULL);	
	
/* Run the program for 2500 rounds */	
while(rounds++ < 2500){	
	#pragma omp parallel shared ( msg, thread_word )  private( tid, start, end ) 
	{	
		/* Get the id of th thread */
		tid = omp_get_thread_num() ;
		
		/* Define the work that each thread will perform */
		start = floor(tid * work / omp_get_num_threads());
		end = floor((tid + 1) * work / omp_get_num_threads());
	
		//printf("start = %d - end = %d - tid = %d \n", start, end, tid);
	
		/* count the number of ones */		
		thread_word[tid] = number_of_ones( start, end );	
	}
}//end of while
	
	/* Final computation after the threads have done their assigned work */
	for(i = 0; i < thread_num; i++)
		num_of_ones += thread_word[i]; 

	printf("NUmber of ones is %d \n", num_of_ones);
	
	/* Time that has elapsed */
	gettimeofday(&tempo2, NULL);
    printf("tempo2.tv_sec = %ld, tempo2_tv_usec = %ld\n", 
            tempo2.tv_sec, tempo2.tv_usec);
    printf("tempo1.tv_sec = %ld, tempo1_tv_usec = %ld\n", 
            tempo1.tv_sec, tempo1.tv_usec);
    elapsed_seconds  = tempo2.tv_sec  - tempo1.tv_sec;
    elapsed_useconds = tempo2.tv_usec - tempo1.tv_usec;

    printf("Elapsed time = %ld seconds + %ld microseconds\n",
            elapsed_seconds, elapsed_useconds);

    //elapsed_utime = (elapsed_seconds) * 1000000 + elapsed_useconds;
    elapsed_mtime = ((elapsed_seconds) * 1000 + elapsed_useconds/1000.0) + 0.5;

    //printf("Elapsed time = %ld microseconds\n", elapsed_utime);
    printf("Elapsed time = %ld milliseconds\n", elapsed_mtime);
	
	/* Free memory */
	free(msg);
	msg = NULL;

	
	//Succesfull exit
	return 0;

}

/* Count the number of ones in a string */
long number_of_ones( start, end )
{	
	//Initialization of variables
	int i = 0;
	long count = 0;
	
	/* Main code that counts the number of ones
	until the end of the string*/
	for( i = start; i < end; i++ )
		if( msg[i] == '1' )
			++count ;
	
	/*for( ; word[i] != 0 ; ++i )
		if( word[i] == '1' )
			++count ;*/
			
	/* Return the result */	
	return count ;
}		
		