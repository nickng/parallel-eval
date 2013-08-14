#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libsess.h>

#define participants 4

/* Function that counts the occurences of ones */
int number_of_ones( const char * );

int main(int argc, char ** argv){

	/* variables declaration */
	int i;
	int *size_of_buffer = 0;
	int num_of_ones = 0;
	
	/* Number of rounds that the algorithm will run */
	int rounds = 0;		
	
	/* buffer that master will send */
	char *buffer = NULL;
	
	/* sessionC handler */
	session *s;
	join_session(&argc, &argv, &s, "Slave3.spr");
	role *Master = s->get_role(s, "Master");	
	
	/* receive size of buffer */
	receive_int(Master, &size_of_buffer);
	
	printf("size of buffer is %d\n", *size_of_buffer);
	
	/* Dynamic memory allocation */
	buffer = (char *) malloc( (*size_of_buffer) * sizeof(char));
	
	/* Receive the buffer */
	receive_string(Master, &buffer);
	
/* For 100000 rounds */	
while(rounds++ < 100000){		
	/* Compute the number of ones */
	num_of_ones = number_of_ones(buffer); 
}	
	
	printf("num of ones is %d\n", num_of_ones);
	
	/* Send the result to the master node */
	send_int(Master, num_of_ones);
	
	/* Deallocate memory */
	free(buffer);
	
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
