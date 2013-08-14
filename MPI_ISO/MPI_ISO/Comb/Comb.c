/*
MPI implementation of the dwarf related to
Combinational logic and specifically couting 
the number of '1' in a word, which is very important
in Data Mining techniques
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

/* The file form where we will read the specified word */
const char *FILE_NAME = "now.txt";

/* Function that counts the occurences of ones */
long number_of_ones( const char * );

int main(int argc, char **argv){

	/* Declaration of variables to measure the time that has elapsed */
	double start_time;
	double end_time;

	//Variable declaration
	int i;
	int j;
	int rows_of_proc;
	int send_count = 0;
	long num_of_ones = 0; 
	long result = 0;
	
	/* number of characters in the file */
	int count = 0;	
	int rounds = 0;
	
	//Mpi variables
	int rank;
	int numtasks;
	int tag = 1;

	/* The word that we will count the occurences of 1 */
	char *msg = NULL;
	
	//Mpi initialization
	MPI_Status Stat;
	MPI_Init ( &argc, &argv );
	MPI_Comm_size ( MPI_COMM_WORLD, &numtasks );
	MPI_Comm_rank ( MPI_COMM_WORLD, &rank );	
	
	/* Barrier in order to start the time counting */
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();
	
	/* master node reads the word from the
	specified file*/
	if(rank == 0){
		//char *msg = NULL;
		
		/* The word that we will read from the file */
		char *file_word = NULL;
	
		/* character or EOF flag from input */
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
	}

	/* Broadcast the count(length of our word)
	to the other processors*/
	MPI_Bcast (&count,1,MPI_INT,0,MPI_COMM_WORLD); 
	
	/* sendcount is the variable that determines the work
	that each processor will do */
	send_count = floor(((rank+1) * count)/numtasks) - floor((rank * count)/numtasks) ;
	
	printf("rank = %d - ssssendcount = %d \n",rank, send_count);
	
	//printf("COUNT IS %d - msg's length is %d\n\n\n", count, strlen(msg));
	/* Declare the sub-string that each processor will
	do the necessary work*/
	char *rec_buffer = NULL;
	rec_buffer = (char *) malloc( send_count * sizeof(char) );
	rec_buffer[send_count-1] = '\0';

	//printf("edwwwwww\n");
	int ok = -2;
	int received = -3;
	
	/* Master process sends the corresponding substring
	to each processor, including itself*/
	if(rank == 0){

		int start = -1;
		int end = -1;
	
		printf("Sending work...\n");
		for( i = 0; i < numtasks; i++){
			start = (int) floor((i * count)/numtasks);
			end = (int)floor(((i+1) * count)/numtasks); //- (int)floor((i * count)/numtasks);
			//printf("Rank is %d - Start is %d - End is %d \n", i, start, end);
			
			ok = MPI_Send(msg + (int) floor((i * count)/numtasks), (int)floor(((i+1) * count)/numtasks) - (int)floor((i * count)/numtasks)
			, MPI_CHAR, i, tag, MPI_COMM_WORLD);
			//printf("Rank is %d - ok is %d\n", i, ok);
		}
	}
	
	/* All the processes receive the sub-array */
	received = MPI_Recv (rec_buffer, send_count, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &Stat);
	//printf("Rank is %d - received is %d\n", rank, received);
	//printf("rank = %d - buffer is %s\n", rank, rec_buffer);
	//printf("EDW\n");
	/* Each processor computes the number of '1'
	of it's given sub-string*/
	while(rounds++ < 100000)
		num_of_ones = number_of_ones( rec_buffer );

	/* Free memory */
	free(msg);
	msg = NULL;
	free(rec_buffer);
	rec_buffer = NULL;
	
	printf("rank = %d - num_of_ones is %ld\n", rank, num_of_ones);

	/* Use of MPI_Reduce to compute the total
	number of '1' from each processor*/
	MPI_Reduce(&num_of_ones, &result, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	
	/* Barrier in order to end the time counting */
	MPI_Barrier(MPI_COMM_WORLD);
	end_time = MPI_Wtime();
	
	/* Master node prints the result */
	if(rank == 0){
		printf("Total number of ones is %ld\n", result);
		printf("Time elapsed is %1.6f\n", end_time - start_time);
	}
	

	
	MPI_Barrier(MPI_COMM_WORLD);

	//MPI_Free_mem(msg);
	//MPI_Free_mem(rec_buffer);
	/* End of MPI */
	MPI_Finalize();	
	

	
	//Succesfull exit
	return 0;

}

/* Count the number of ones in a string */
long number_of_ones( const char *word )
{	
	//Initialization of variables
	int i = 0;
	long count = 0;
	
	/* Main code that counts the number of ones
	until the end of the string*/
	for( ; word[i] != 0 ; ++i )
		if( word[i] == '1' )
			++count ;
			
	/* Return the result */	
	return count ;
}

