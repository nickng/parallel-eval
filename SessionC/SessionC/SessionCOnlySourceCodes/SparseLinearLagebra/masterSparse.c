/*
Parallel computation of Compressed Sparse Row
Algorithm(A*x = y) using SrssionC 
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libsess.h>
//#include <zmq.h>

//#define nrows 1000
//#define ncolumns 1000

int nrows = 1000;
int ncolumns = 1000;

#define participants 4
#define master 0

/* Function that prints the matrix */
void print_array(int **);

/* Function that prints vector */
void print_vector(int *, int );

/* Function that finds the non zero
elements of an array */
int num_of_non_zero_elements(int **);

/* Fill tha matrix */
void fill_matrix(int **);

/* Function that fills the values array */
void fill_values(int **, int *);

/* Function that fills the col_ind array */
void fill_col_ind(int **, int *);

/* Function that fills the row_ptr array */
void fill_row_ptr(int **, int *);

/* Function that fills a vector */
void fill_vector(int *, int);

/* Fill the array that holds the results with zeros */
void fill_with_zeros(int *, int);


int main(int argc ,char **argv){

	/* Declare start and end of computation time */
	struct timeval start, end;

	/* Variables declaration */
	int i;
	int j;
	int rows_of_proc;

	/* Number of rounds that the parallel part will be computed */
	int rounds = 0;
	
	/* Variable that shows the non zero elemets of the array */
	int non_zero_elements = 0;	

	/* Session start */
	session *s;
	join_session(&argc, &argv, &s, "Master.spr");
	role *worker1 = s->get_role(s, "worker1");		
	role *worker2 = s->get_role(s, "worker2");
	role *worker3 = s->get_role(s, "worker3");
	
	/* Declaration of array results, which is the vector 
	that will hold the results */
	int *results = NULL;
	
	/* Declaration of array x, which is the vector 
	that will bw multiplied with the array */
	int *x = NULL;
	//Dynamic memory allocation
	x = (int *) malloc( ncolumns * sizeof(int) );
	if(x == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}
	
	//Fill the vector x
	printf("Filling and printing vector x \n");
	fill_vector(x, ncolumns);
	
	
	/* The array that contains the start of each row within
	the non-zero elements array*/
	int *row_ptr = NULL;
	
	/* Dynamic memory allocation of the array row_ptr */
	row_ptr = (int *) malloc( (nrows + 1) * sizeof(int) );
	/* Check if we have enough memory */
	if(row_ptr == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}	

	//Declaration of array A
	int **A = NULL;
		
	//Dynamic memory allocation of the matrix
	A = malloc(nrows * sizeof(int *));
	
	/* Check if we have enough memory */
	if(A == NULL)
	{
	fprintf(stderr, "out of memory\n");
	exit(-1);
	}
	for(i = 0; i < nrows; i++)
	{
		A[i] = malloc(ncolumns * sizeof(int));
		/* Check if we have enough memory */
		if(A[i] == NULL)
		{
			fprintf(stderr, "out of memory\n");
			exit(-1);
		}
	}

	
	/* Fill tha matrix with values */
	fill_matrix(A);

	/* Start counting the time */	
	gettimeofday(&start, NULL);
	
	/* Computation of non zero elements */
	non_zero_elements = num_of_non_zero_elements(A);	
	printf("Num of non zeros is %d \n", non_zero_elements);	
	
	/* Master sends the non zero elements to the workers */
	send_int(worker1, non_zero_elements);
	send_int(worker2, non_zero_elements);
	send_int(worker3, non_zero_elements);
	
	
	
	/* Dynamically allocate memory for the array results */
	results = (int *) malloc( nrows * sizeof(int) );
	fill_with_zeros(results, nrows);	
	
	/* Declaration af array values and memory allocation */
	int *values = (int *) malloc(non_zero_elements * sizeof(int));
	
	/* Check if we have enough memory */
	if(values == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}		

	/* Fill the values array */
	fill_values(A, values);	
	
	/* Declaration of the array values and dynamic memory allocation  */
	int *col_ind = (int *) malloc( non_zero_elements * sizeof(int) );
	
	/* Check if we have enough memory */
	if(col_ind == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}	
	
	/* Fill the col_ind array */
	fill_col_ind(A, col_ind);
	
	/* Fill the row_ptr array with zero*/
	fill_with_zeros(row_ptr, nrows + 1);
	
	/* Print row_ptr initialized */
	////////////////print_vector(row_ptr, nrows + 1);
	
	/* Fill the row_ptr array */
	fill_row_ptr(A, row_ptr);	
	

	
	/* Master sends tha arrays to workers */
	send_int_array(worker1, row_ptr, nrows + 1);
	send_int_array(worker1, values, non_zero_elements);
	send_int_array(worker1, col_ind, non_zero_elements);
	send_int_array(worker1, x, ncolumns);
	
	send_int_array(worker2, row_ptr, nrows + 1);
	send_int_array(worker2, values, non_zero_elements);
	send_int_array(worker2, col_ind, non_zero_elements);
	send_int_array(worker2, x, ncolumns);
	
	send_int_array(worker3, row_ptr, nrows + 1);
	send_int_array(worker3, values, non_zero_elements);
	send_int_array(worker3, col_ind, non_zero_elements);
	send_int_array(worker3, x, ncolumns);	
	
	
	
	/*
	print_vector(values, non_zero_elements);
	printf("\n");
	print_vector(row_ptr, nrows + 1);	
	printf("\n");
	print_vector(col_ind, non_zero_elements);
	*/

	//print_vector(values, non_zero_elements);
	
	/* Define the work that master will do */
	/*int start_work_i[participants];
	int end_work_i[participants];*/
	
	/* The results that workers will send to master */
	int *buffer_results[participants] = {NULL};	
	

	int start_work[participants];
	int end_work[participants];	
	
	int amount_of_work[participants];
	
	/* For all participants */
	for(i = 0; i < participants; i++){
	
		/* The amount of work that the master will do */
		start_work[i] = floor((i * nrows)/participants);
		end_work[i] = floor(((i + 1) * nrows)/participants);	
		
		//printf("I am here\n");
		
		amount_of_work[i] = end_work[i] - start_work[i];
	
		//printf("I am here again\n");
	
		//printf("start = %d - end = %d - amount of work = %d\n", start_work[i], end_work[i], amount_of_work[i]);

		/* Dynamic allocation of buffer results */
		buffer_results[i] = (int *) malloc( (amount_of_work[i]) * sizeof(int));
		if(buffer_results[i] == NULL){
			fprintf(stderr, "Out of memory, aborting program...\n");
		}
	}
	
	/* Buffers that hold the results that the workers send */
	int *buffer_worker1 = (int *) malloc(amount_of_work[1] * sizeof(int));
	int *buffer_worker2 = (int *) malloc(amount_of_work[2]* sizeof(int));
	int *buffer_worker3 = (int *) malloc(amount_of_work[3]* sizeof(int));
	
	
	/* The amount of work that the master will do */
	//start_work[master] = floor((master * nrows)/participants);
	//end_work[master] = floor(((master + 1) * nrows)/participants);	
	
	printf("start = %d - end = %d - amount of work is %d\n", start_work[master], end_work[master], amount_of_work[master]);

/* Run for 100 rounds */	
while(rounds++ < 10000){	
	/* Main computation of the result. Each processor
	computes the work that is assigned to it*/
	for(i = start_work[master]; i < end_work[master]; i++){
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			buffer_results[0][i] += values[j] * x[col_ind[j]];
			//results[i] += values[j] * x[col_ind[j]];
	}
}	
	
	//int *amount = 15000;

	//int *work1 = NULL;
	//int *work2 = NULL;
	//int *work3 = NULL;
	
	//work1 = amount_of_work[1];
	//work2 = amount_of_work[2];
	//work3 = amount_of_work[3];
	
	//printf("work1 is %d\n", work1);
	//printf("here?\n");
	
	/*  */
	size_t work1 = amount_of_work[1];
	size_t work2 = amount_of_work[2];
	size_t work3 = amount_of_work[3];
	
	/* Master node receives the results */
	recv_int_array(worker1, buffer_worker1, &work1);
	//printf("Received array from worker1\n");
	//printf("amount of work 1 is %d\n", amount_of_work[1]);
	recv_int_array(worker2, buffer_worker2, &work2);
	//printf("Received array from worker2\n");
	recv_int_array(worker3, buffer_worker3, &work3);
	//printf("Received array from worker3\n");
	
	/*receive_int_array(worker1, buffer_results[1], &work1);
	receive_int_array(worker2, buffer_results[2], &work2);
	receive_int_array(worker3, buffer_results[3], &work3);*/
	
	/*receive_int_array(worker1, buffer_results[1], amount_of_work[1]);
	receive_int_array(worker2, buffer_results[2], amount_of_work[2]);
	receive_int_array(worker3, buffer_results[3], amount_of_work[3]);*/
	
	//printf("I am here\n");
	
	/*receive_int_array(worker1, buffer_results[1], amount_of_work[1]);
	receive_int_array(worker2, buffer_results[2], amount_of_work[2]);
	receive_int_array(worker3, buffer_results[3], amount_of_work[3]);*/
	
	/* End counting of time here */
	gettimeofday(&end, NULL);
	
	/* Computation of time */
	long time_elapsed = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000;
	 
	/* Print the time that has elapsed */
	printf("Elapsed time is: %ld seconds\n", time_elapsed);	
	
	/* End current session */
	end_session(s);	
	
/*
int counter = 0;	
while(counter++ < 1000){
	for(i = 0; i < nrows; i++){
		//printf("rank = %d - i = %d\n", rank, i);
		//printf("RANK = %d\n", rank);
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			results[i] += values[j] * x[col_ind[j]];
			//results[i - start] += values[j] * x[col_ind[j]];
	}

}*/
	
	/* Define the sub-buffers that we will send to the participants */
	/*int *buffer_values[participants] = {NULL};
	int *buffer_col_ind[participants] = {NULL};
	int *buffer_row_ptr[participants] = {NULL};*/
	
	/* Compute which part of the array each participant will compute */
	//for(i = 0; i < participants; i++){
		/*start_work_i[i] = floor((i * nrows)/participants);
		end_work_i[i] = floor(((i + 1) * nrows)/participants);

		start_work[i] = floor((i * non_zero_elements)/participants);
		end_work[i] = floor(((i + 1) * non_zero_elements)/participants);		

		amount_of_work[i] = floor(((i + 1) * non_zero_elements)/participants) - floor(( i * non_zero_elements)/participants);
		//amount_of_work[i] = end_work[i] - start_work[i];
		printf("start = %d - end = %d - amount of work = %d\n", start_work[i], end_work[i], amount_of_work[i]);
		*/
		/* Dynamically buffer allocation */
		/*buffer_values[i] = (int *) malloc( (amount_of_work[i]) * sizeof(int));
		buffer_col_ind[i] = (int *) malloc( (amount_of_work[i]) * sizeof(int));
		*/
		//buffer_values[i] = (int *) malloc( (non_zero_elements) * sizeof(int));
		//buffer_col_ind[i] = (int *) malloc( (non_zero_elements) * sizeof(int));
		
		/* Check if memory is enough */
		/*if(buffer_values[i] == NULL){
			fprintf(stderr, "Out of memory.Aborting the program...\n");
			exit(0);
		}*/

		/* Check if memory is enough */
		/*if(buffer_col_ind[i] == NULL){
			fprintf(stderr, "Out of memory.Aborting the program...\n");
			exit(0);
		}*/	
	//}

	////////////////////////print_array(A);
	////////////////////////print_vector(row_ptr, nrows + 1);
	
	//printf("row_ptr[%d] = %d\n", start_work[0], row_ptr[start_work[0]]);
	//printf("row_ptr[%d] = %d\n", end_work[0], row_ptr[end_work[0]]);
	
	//print_vector(buffer_values[0], amount_of_work[0]);
	//printf("\n\n");
	
	
	
	/* Compute the buffer for each participant (amount of work) */
	/*for(i = 0; i < participants; i++){
		memcpy( buffer_values[i], values + start_work[i], amount_of_work[i] * sizeof(int) );
		memcpy( buffer_col_ind[i], col_ind + start_work[i], amount_of_work[i] * sizeof(int));
		//printf("buffer[%d] = %s\n", i, buffer[i]);
	}*/	
	
	//print_vector(buffer_values[0], amount_of_work[0]);
	//print_vector(buffer_col_ind[0], amount_of_work[0]);
	//print_vector(col_ind, non_zero_elements);
	//print_vector(values, non_zero_elements);
	
	////////print_vector(col_ind, non_zero_elements);
	
	/* Main computation of the result. Master
	computes the work that is assigned to it*/
	/*for(i = start_work[0]; i < end_work[0]; i++){
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			results[i] += values[j] * x[col_ind[j]];
			//results[i - start] += values[j] * x[col_ind[j]];
	}*/
	/*
	for(i = start_work_i[1]; i < end_work_i[1]; i++){
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++){
			
			printf("j = %d\n", j);
			results[i] += buffer_values[1][j] * x[buffer_col_ind[1][j]];
		}	
	}	*/
		
	//printf("buffer_values[%d][%d] = %d\n", 1, 12, buffer_values[1][12]);	
		
	/* Main computation of the result. Each processor
	computes the work that is assigned to it*/
	/*for(i = start; i < end; i++){
		//printf("rank = %d - i = %d\n", rank, i);
		//printf("RANK = %d\n", rank);
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			y[i] += values[j] * x[col_ind[j]];
			//results[i - start] += values[j] * x[col_ind[j]];
	}*/	
	
	
	/* The arrays that we will send to the workers */
	//int *buffer_col_ind = NULL;
	//int *buffer_values = NULL;

	/* Free memory */
	free(x);
	free(results);
	free(values);
	free(col_ind);
	free(row_ptr);
	free(buffer_worker1);
	free(buffer_worker2);
	free(buffer_worker3);
	
	return EXIT_SUCCESS;
}


/* Function that prints the matrix */
void print_array(int **array){
	//Variables declaration
	int i,j;
	
	for(i = 0; i < nrows; i++){
		for(j = 0; j < ncolumns; j++)
			printf(" A[%d][%d] = %d ", i, j, array[i][j]);
		printf("\n");	
	}
}

/* Function that prints vector */
void print_vector(int *vector, int length){
	//Variables declaration
	int i = 0;
	
	for(i = 0; i < length; i++)
		printf(" %d ", vector[i]);
	printf("\n\n");
}

/* Function that finds the non zero
elements of an array */
int num_of_non_zero_elements(int **array){
	//Variables declaration
	int i,j;
	int non_zeros = 0;
	
	
	for(i = 0; i < nrows; i++)
		for(j = 0; j < ncolumns; j++)
			if(array[i][j] != 0)
				non_zeros ++;
	
	return non_zeros;
}

/* Fill tha matrix */
void fill_matrix(int **array){

	//Variables declaration
	int i,j;
	int non_zero_flag = 0;
	
	for(i = 0; i < nrows; i++){
		for(j = 0; j < ncolumns; j++){
			array[i][j] = rand() % 2;
			if(array[i][j] != 0)
				non_zero_flag = 1;
			if(non_zero_flag == 0){
				array[i][0] = 1;
				non_zero_flag = 0;
			}	
		}		
	}
}

/* Function that fills the values array */
void fill_values(int **array, int *value){
	//Variables declaration
	int i,j;
	//Index of the value array
	int counter = 0;

	for(i = 0; i < nrows; i++)
		for(j = 0; j < ncolumns; j++)
			if(array[i][j] != 0){
				//printf(" value = %d ", array[i][j]);
				value[counter++] = array[i][j];
			}
}

/* Function that fills the col_ind array */
void fill_col_ind(int **array, int *col_inds){

	//Variables declaration
	int i,j;
	//the non zeros in a column
	int cols = 0;
	//The position of the index in col_inds array
	int index = 0;
	
	for(i = 0; i < nrows; i++){
		for(j = 0; j < ncolumns; j++){
			if(array[i][j] != 0)
				col_inds[index++] = cols;
			cols++;	
		}		
		//Make cols 0 after iteration
		cols = 0;
	}	
}

/* Function that fills the row_ptr array */
void fill_row_ptr(int **array, int *rows_ptr){

	//Variable declaration
	int i, j;
	//Variable that shows if we are in a new row
	int new_row = 0;
	//Num of non zero elements
	int num_of_non_zeros = 0;
	//Index for the array rows_ptr
	int index = 0;
	/* Check for each row the first non zero element */
	for(i = 0; i < nrows; i++){
		new_row = 1;
		for(j = 0; j < ncolumns; j++){
			if(array[i][j] != 0){
				if(new_row == 1){
					rows_ptr[index++] = num_of_non_zeros;
					
					//printf("index is %d\n", index);
					new_row = 0;
				}	
				num_of_non_zeros++;
			}	
		}		
	}	

	//Fill the last position of the array
	rows_ptr[nrows] = num_of_non_zeros;  	
}

/* Function that fills a vector */
void fill_vector(int *vector, int length){

	//Variable declaration
	int i;
	
	/* Fill the array with random values */
	for(i = 0; i < length; i++)
		vector[i] = rand() % 10;
}

/* Fill the array that holds the results with zeros */
void fill_with_zeros(int *vector, int length){
	
	//Variable declaration
	int i;

	for(i = 0; i < length; i++)
		vector[i] = 0;

}
