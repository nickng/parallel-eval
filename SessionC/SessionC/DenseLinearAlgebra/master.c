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
#define master 0

/* Function that prints a 2D array */
void print_matrix(int **array);

/* Function that fills a 2D array */
void fill_matrix(int **array);

/* Function that prints vector */
void print_vectors(int *, int );

/* Function that fills a vector */
void fill_vector(int *, int);

/* Fill the array that holds the results with zeros */
void fill_with_zeros(int *, int);

int main(int argc, char **argv){

	/* Declare start and end of computation time */
	struct timeval start, end;

	/* Variables declaration */
	int i;
	int j;

	/* rounds that we will run the algorithm */
	int rounds = 0;
	
	/* The work that each participant will do */
	int start_work[participants] = {0};
	int end_work[participants] = {0};
	int amount_of_work[participants] = {0};
	
	/* Total size of the array Beta */
	int total_size = rows * columns;
	
	/* Session start */
	session *s;
	join_session(&argc, &argv, &s, "master.spr");
	role *worker1 = s->get_role(s, "worker1");		
	role *worker2 = s->get_role(s, "worker2");
	role *worker3 = s->get_role(s, "worker3");	
	
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
	int *Beta = NULL;
	
	/* Dynamic memory allocation */
	Beta = (int *) malloc( total_size * sizeof(int) );
	/* Abort if there is not enough memory */
	if(Beta == NULL){
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}
	
	/* Fill Beta vector */
	fill_vector(Beta, total_size);
	/* Fill matrix C with random values */
	fill_vector(C, columns);		
	
	/* Compute the work that each participant will perform */
	for(i = 0; i < participants; i++){
		start_work[i] = floor( (i * rows) / participants );
		end_work[i] = floor( ((i + 1) * rows) / participants );
		amount_of_work[i] = end_work[i] - start_work[i];
		printf("start work is %d - end work is %d - amount of work is %d\n", start_work[i], end_work[i], amount_of_work[i]);
	}
	
	/* Declaration of the arrays that will hold the results */
	int *master_results = NULL;
	int *worker1_results = NULL;
	int *worker2_results = NULL;
	int *worker3_results = NULL;
	int *results = NULL;
	
	/* Dynamic memory allocation of the arrays that will hold the results */
	master_results = (int *) malloc(amount_of_work[master] * sizeof(int));
	worker1_results = (int *) malloc(amount_of_work[1] * sizeof(int));
	worker2_results = (int *) malloc(amount_of_work[2] * sizeof(int));
	worker3_results = (int *) malloc(amount_of_work[3] * sizeof(int));
	results = (int *) malloc(rows * sizeof(int));
	
	/* Fill the results array with zero */
	fill_with_zeros(master_results, amount_of_work[master]);
	fill_with_zeros(worker1_results, amount_of_work[1]);
	fill_with_zeros(worker2_results, amount_of_work[2]);
	fill_with_zeros(worker3_results, amount_of_work[3]);
	
	/* Start counting the time */	
	gettimeofday(&start, NULL);	
	
	/* Master sends the arrays to workers */
	send_int_array(worker1, C, columns);
	send_int_array(worker1, Beta + total_size / participants, total_size / participants);
	
	send_int_array(worker2, C, columns);
	send_int_array(worker2, Beta + 2 * (total_size / participants), total_size / participants);
	
	send_int_array(worker3, C, columns);
	send_int_array(worker3, Beta + 3 * (total_size / participants), total_size / participants);	
	
/* Run for 1000 rounds */	
while(rounds++ < 1000){	
	/* Main computation from master */
	for(i = 0; i < amount_of_work[master]; i++)
		for(j = 0; j < columns; j++)
			master_results[i] += Beta[i * amount_of_work[master] + j] * C[j];
}//End of while
	
	/* The size of the arrays that master waits from the workers */
	size_t work1 = amount_of_work[1];
	size_t work2 = amount_of_work[2];
	size_t work3 = amount_of_work[3];	
	
	/* Master node receives the results */
	recv_int_array(worker1, worker1_results, &work1);
	printf("Received array from worker1\n");
	//printf("amount of work 1 is %d\n", amount_of_work[1]);
	recv_int_array(worker2, worker2_results, &work2);
	printf("Received array from worker2\n");
	recv_int_array(worker3, worker3_results, &work3);
	printf("Received array from worker3\n");
	
	/* Copy the results from all participants to the array results */
	memcpy(results + start_work[master], master_results, amount_of_work[master]);
	memcpy(results + start_work[1], worker1_results, amount_of_work[0]);
	memcpy(results + start_work[2], worker2_results, amount_of_work[1]);
	memcpy(results + start_work[3], worker3_results, amount_of_work[2]);
	
	/* End counting of time here */
	gettimeofday(&end, NULL);
	
	long time_elapsed = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000;
	 
	/* Print the time that has elapsed */
	printf("Elapsed time is: %ld seconds\n", time_elapsed);	
	
	/* End Session */
	end_session(s);
	
	/* Free memory */	
	free(C);
	C = NULL;
	
	free(Beta);	
	Beta = NULL;
	
	free(master_results);
	master_results = NULL;
	
	free(worker1_results);
	worker1_results = NULL;	
	
	free(worker2_results);
	worker2_results = NULL;	

	free(worker3_results);
	worker3_results = NULL;		
	
	free(results);
	results = NULL;
	
	return EXIT_SUCCESS;
}

//Print array
void print_matrix(int **array){

	int i, j;
	
	for( i = 0; i < rows; i++ ){
		for( j = 0; j < columns; j++ ){
			printf("B[%d][%d] = %d \n", i, j, array[i][j]);
		}	
		printf("\n");
	}
}

/* Fill the matrix with random values */
void fill_matrix(int **array){

	/* Variable declaration */
	int i, j;
	
	for( i = 0; i < rows; i++ ){
		for( j = 0; j < columns; j++ ){
			array[i][j] = rand() % 10;
		}	
		//printf("\n");
	}
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