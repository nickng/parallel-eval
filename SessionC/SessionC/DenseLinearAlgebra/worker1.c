/* SessionC implementation of the Dense 
Linear Algebra dwarf */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <libsess.h>

/* Declare heigth and width of the arrays */
#define rows 1000 
#define columns 1000

#define participants 4
#define worker1 1

/* Function that prints vector */
void print_vectors(int *, int );

/* Function that fills a vector */
void fill_vector(int *, int);

/* Fill the array that holds the results with zeros */
void fill_with_zeros(int *, int);

int main(int argc, char **argv){

	/* Variables declaration */
	int i;
	int j;
	
	/* Number of rounds to run the algorithm */
	int rounds = 0;
	
	/* Total size of the array */
	int total_size = (rows * columns) / participants;
	
	printf("Total amount of work is %d\n", total_size);
	
	/* the amount of work that is assigned to worker1 */
	int amount_of_work = floor( (worker1 + 1) * columns / participants ) - floor( worker1 * columns / participants );
	
	printf("amount of work is %d\n", amount_of_work);
	
	/* Session start */
	session *s;
	join_session(&argc, &argv, &s, "worker1.spr");
	role *master = s->get_role(s, "master");
	
	/* Dynamic memory allocation of array C */
	int *C = NULL;
	C = (int *) malloc(columns * sizeof(int));
	
	if(C == NULL)
	{
		/* Terminate program if there is not enough memory */ 
		fprintf(stderr, "out of memory \n");
		exit(0);
	}

	/* Declaration of the main array */
	int *Beta_worker1 = NULL;
	
	/* Dynamic memory allocation */
	Beta_worker1 = (int *) malloc( total_size * sizeof(int) );
	
	/* Abort if there is not enough memory */
	if(Beta_worker1 == NULL){
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}	
	
	/* The array that will hold the results */
	int *worker1_results = NULL;
	worker1_results = (int *) malloc( amount_of_work * sizeof(int) );
	
	/* Fill with zeros the result array */
	fill_with_zeros(worker1_results, amount_of_work);
	
	/* The size of the arrays that master waits from the workers */
	size_t array_C = columns;
	size_t array_Beta = total_size;	
	
	/* worker1 receives the arrays in order to do the computation */
	recv_int_array(master, C, &array_C);
	recv_int_array(master, Beta_worker1, &array_Beta);
	
/* Run for 1000 rounds */	
while(rounds++ < 1000){	
	/* Main computation from master */
	for(i = 0; i < amount_of_work; i++)
		for(j = 0; j < columns; j++)
			worker1_results[i] += Beta_worker1[i * amount_of_work + j] * C[j];
}//End of while	
	
	/* worker1 sends the results to the master */
	send_int_array(master, worker1_results, amount_of_work);
	
	/* End Session */
	end_session(s);	
	
	/* Deallocate memmory */
	free(C);
	C = NULL;
	free(Beta_worker1);
	Beta_worker1 = NULL;
	free(worker1_results);
	worker1_results = NULL;
	
	return EXIT_SUCCESS;
}

/* Function that fills a vector */
void fill_vector(int *vector, int length){

	//Variable declaration
	int i;
	
	/* Fill the array with random values */
	for(i = 0; i < length; i++)
		vector[i] = rand() % 10;
}

/* Function that prints vector */
void print_vectors(int *vector, int length){
	//Variables declaration
	int i = 0;
	
	for(i = 0; i < length; i++)
		printf(" %d ", vector[i]);
	printf("\n\n");
}


/* Fill the array that holds the results with zeros */
void fill_with_zeros(int *vector, int length){
	
	/* Variable declaration */
	int i;

	for(i = 0; i < length; i++)
		vector[i] = 0;

}	