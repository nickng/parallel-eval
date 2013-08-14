#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libsess.h>

#define participants 4
#define Master 0

/* The file form where we will read the specified word */
const char *FILE_NAME = "input.txt";

/* Function that counts the occurences of ones */
int number_of_ones( const char * );

/* Number of characters in the file */
int num_of_chars();

/* Read the word from the file */
char *read_word(int);

/* input file */
FILE *in_file;
		

int main(int argc, char ** argv){

	/* Declare start and end of computation time */
	struct timeval start, end;

	/* Variable declaration */
	int i;
	int j;
	int rows_of_proc;
	int send_count = 0;
	//long num_of_ones = 0; 
	long result = 0;
	
	/* number of characters in the file */
	int count = 0;	
	int total_ones = 0;
	
	/* Number of rounds for the computation */
	int rounds = 0;
	
	/* Number of 1 that each participant has computed */
	int num_of_ones[participants] = {0};
	/*int *num_from_slave1 = 0;
	int *num_from_slave2 = 0;
	int *num_from_slave3 = 0;*/
	
	int *slaves[participants - 1] = {NULL};
	
	/* Define the work that each participant will do */
	int start_work[participants];
	int end_work[participants];
	int amount_of_work[participants];
	
	/* The word that we will count the occurences of 1 */
	char *msg = NULL;
	
	/* Each participant will do his own work */
	char *buffer1 = NULL;
	char *buffer2 = NULL;
	char *buffer3 = NULL;
	char *buffer4 = NULL;
	char *buffer[participants] = {NULL};
	
	/* sessionC handler */
	session *s;
	join_session(&argc, &argv, &s, "Master.spr");
	role *Slave1 = s->get_role(s, "Slave1");
	role *Slave2 = s->get_role(s, "Slave2");	
	role *Slave3 = s->get_role(s, "Slave3");
	
	
	/* Get the number of characters that the file has */
	count = num_of_chars();
	//printf("count =  %d\n", count);	

	/* The word that we count the
	occurences of 1 - Dynamic memory allocation*/
	msg = (char *) malloc( (count ) * sizeof(char) );	
	
	if(msg == NULL){
		fprintf(stderr, "Out of memory.Aborting the program...\n");
		exit(0);
	}	
	
	/* Read the word from the file */
	msg = read_word(count);
	
	//print the message
	//printf("msg is %s\n", msg);
	
	/* Start counting the time */	
	gettimeofday(&start, NULL);	
	
	/* Compute which part of the word each participant will compute */
	for(i = 0; i < participants; i++){
		start_work[i] = floor((i * count)/participants);
		end_work[i] = floor(((i + 1) * count)/participants);
		amount_of_work[i] = end_work[i] - start_work[i];
		//printf("start = %d - end = %d - amount of work = %d\n", start_work[i], end_work[i], amount_of_work[i]);
		
		/* Dynamically buffer allocation */
		buffer[i] = (char *) malloc( (amount_of_work[i]) * sizeof(char));
		
		if(buffer[i] == NULL){
			fprintf(stderr, "Out of memory.Aborting the program...\n");
			exit(0);
		}
		
	}
	
	/* Master sends the buffer size to the participants */
	send_int(Slave1, amount_of_work[1]);
	//send_int(Slave1, amount_of_work[1]);
	send_int(Slave2, amount_of_work[2]);
	send_int(Slave3, amount_of_work[3]);
	
	/* Compute the buffer for each participant (amount of work) */
	for(i = 0; i < participants; i++){
		memcpy( buffer[i], msg + start_work[i], amount_of_work[i] - 1);
		//buffer[amount_of_work[i]] = '\0';
		//printf("buffer[%d] = %s\n", i, buffer[i]);
	}	
	
	//printf("buffer[%d] = %s\n", 0, buffer[0]);
	//printf("okkkkkkkkk\n");
	/* Master sends the work to each slave(worker) */
	send_string(Slave1, buffer[1]);
	send_string(Slave2, buffer[2]);
	send_string(Slave3, buffer[3]);
	//printf("oaaaaaaaaaaaaaaaaaa\n");
/* For 100000 rounds */	
while(rounds++ < 100000){	
	/* Master computes its own part */
	num_of_ones[Master] = number_of_ones( buffer[Master] );
}	
	////
	//num_from_slave1 = num_of_ones[Master];
	//printf("num_from_slave1 is %d\n", num_from_slave1);
	
	//printf("obbbbbbbbbbbbbbb\n");
	//printf("Number of ones is %d\n", num_of_ones[Master]);
	
	/* Master receives the number of ones form the other participants */
	/*receive_int(Slave1, &num_from_slave1);
	receive_int(Slave2, &num_from_slave2);
	receive_int(Slave3, &num_from_slave3);*/

	receive_int(Slave1, &slaves[0]);
	receive_int(Slave2, &slaves[1]);
	receive_int(Slave3, &slaves[2]);
	printf("num form slave 1 is %d\n", *slaves[0]);
	
	//receive_int(Slave1, &num_of_ones[1]);
	//receive_int(Slave2, &num_of_ones[2]);
	//receive_int(Slave3, &num_of_ones[3]);
	
	/* End counting of time here */
	gettimeofday(&end, NULL);
	
	/* Computation of time */
	long time_elapsed = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000;
	 
	/* Print the time that has elapsed */
	printf("Elapsed time is: %ld seconds\n", time_elapsed);
	
	/* Print the number of ones */
	//for(i = 0; i < participants; i++)
		//printf("Num of ones from Participant %d is %d\n", i, num_of_ones[i]);
	
	/* print total number of ones that have been computed */
	//for(i = 0 ; i < participants; i++)
		//total_ones = num_of_ones[Master] + *num_from_slave1 + *num_from_slave2 + *num_from_slave3;
		
	/* Compute and print the total number of ones */	
	total_ones = num_of_ones[Master] + *slaves[0] + *slaves[1] + *slaves[2];
	printf("Total number of ones is %d\n", total_ones);	
	
	/* Free memory */
	for(i = 0; i < participants; i++){
		free(buffer[i]);
	}	
	for(i = 0; i < participants - 1; i++){
		free(slaves[i]);
	}		
	free(msg);
	
	/* End current session */
	end_session(s);
	
	
	return EXIT_SUCCESS;
}

/* Count the number of ones in a string */
int number_of_ones( const char *word )
{	
	//Initialization of variables
	int i = 0;
	int count = 0;
	int j = 4;
	
	/* Main code that counts the number of ones
	until the end of the string*/
	for( ; word[i] != 0 ; ++i )
		if( word[i] == '1' ){
			++count ;
		}		
	/* Return the result */	
	return count ;
}

/* Number of characters in the file */
int num_of_chars(){

	/* number of characters in the file */
	int count = 0;
	
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
	
	//printf("Number of characters in %s is %d\n",
	//FILE_NAME, count);
		
	/* Close the file */
	fclose(in_file);

	return count;	
}

/* Read the word from the file */
char *read_word(int count){

	/* character or EOF flag from input */
	int ch;

	/* The word that we will count the occurences of 1 */
	char *msg = NULL;

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
	msg[counter] = '\0';
	
	/* Close the file */
	fclose(in_file);	

	//printf("Word is function is %s\n", msg);	

	return msg;
}
