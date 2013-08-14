/*
Parallel computation of Compressed Sparse Row
Algorithm(A*x = y) using MPI 
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define nrows 10000
#define ncolumns 10000

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

	/* Declaration of variables to measure the time that has elapsed */
	double start_time;
	double end_time;

	//Variables declaration
	int i;
	int j;
	int rows_of_proc;
	int start;
	int rounds = 0;
	
	//Variable that shows the non zero elemets of the array
	int non_zero_elements = 0;	
	
	//Mpi variables
	int rank;
	int numtasks;
	int tag = 1;	
	
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
	////////////////////////////////////////////////print_vector(x, ncolumns);	
	printf("\n");	
	
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
	
	/* The array that will hold the non zero values */
	int *values = NULL;
	
	/* The array that will hold the indexes of the columns */
	int *col_ind = NULL;
	
	/* Declaration of array y, which is the vector 
	that will hold the results */
	int *y = NULL;
	y = (int *) malloc( nrows * sizeof(int) );
	fill_with_zeros(y, nrows);	
	
	/* The sub - results of the work of each processor */
	//int *results = NULL;
	
	//Mpi initialization
	MPI_Status Stat;
	MPI_Init ( &argc, &argv );
	MPI_Comm_size ( MPI_COMM_WORLD, &numtasks );
	MPI_Comm_rank ( MPI_COMM_WORLD, &rank );	

			/* Barrier in order to start the time counting */
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();
	
	
	/* The master node computes the number of non zero values
	of the array and the arrays row_ptr, col_ind, values
	that are needed to compute the result*/
	if(rank == 0){
	
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

	fill_matrix(A);		
	
		
	//Computation of non zero elements;
	non_zero_elements = num_of_non_zero_elements(A);	
	//printf("Num of non zeros is %d \n", non_zero_elements);	
	
	/* Dynamic memory allocation of the array values */
	values = (int *) malloc( non_zero_elements * sizeof(int) );
	/* Check if we have enough memory */
	if(values == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}		
	
	/* Dynamic memory allocation of the array values */
	col_ind = (int *) malloc( non_zero_elements * sizeof(int) );
	/* Check if we have enough memory */
	if(col_ind == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}

	//printf("\n");
	/* Fill the values array */
	fill_values(A, values);
	/* Print the values array */
	//printf("Printing values array:\n");
	////////////////////////////////print_vector(values, non_zero_elements);
	
	//printf("\n");
	/* Print the col_ind */
	fill_col_ind(A, col_ind);
	//printf("Printing col_ind array:\n");
	///////////////////////////////print_vector(col_ind, non_zero_elements);
	
	/* Fill the row_ptr array */
	fill_row_ptr(A, row_ptr);
	
	//printf("\n");
	/* Print the values array */
	//printf("Printing row_ptr array:\n");
	///////////////////////////////print_vector(row_ptr, nrows + 1);	
	//printf("\n");
	
	}

	/*Broadcast the number of zeros that the master node found*/ 
	MPI_Bcast (&non_zero_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);		
	//printf("rank = %d - num_of_zeros = %d \n", rank, non_zero_elements);
	
	/* Dynamic memory allocation of the arrays values 
	and col_ind for the other processors */
	if(rank != 0){
	
		/* Dynamic memory allocation of the array values */
		values = (int *) malloc( non_zero_elements * sizeof(int) );
		/* Check if we have enough memory */
		if(values == NULL)
		{
			fprintf(stderr, "out of memory\n");
			exit(-1);
		}			
	
		/* Dynamic memory allocation of the array values */
		col_ind = (int *) malloc( non_zero_elements * sizeof(int) );
		/* Check if we have enough memory */
		if(col_ind == NULL)
		{
			fprintf(stderr, "out of memory\n");
			exit(-1);
		}
	}
	
	/* Wait for the processors to allocate the necessary memory */
	MPI_Barrier(MPI_COMM_WORLD);

	/*Broadcast the arrays row_ptr, values, col_ind to the other nodes*/ 
	MPI_Bcast (row_ptr, nrows + 1, MPI_INT, 0, MPI_COMM_WORLD);		
	MPI_Bcast (col_ind, non_zero_elements, MPI_INT, 0, MPI_COMM_WORLD);	
	MPI_Bcast (values, non_zero_elements, MPI_INT, 0, MPI_COMM_WORLD);
	
	/* print the arrays */
	if(rank != 0){
		//printf("Values: \n");
		////////////////////print_vector(values, non_zero_elements);
		//printf("Col_ind: \n");
		////////////////////print_vector(col_ind, non_zero_elements);
		//printf("Row_ptr: \n");
		////////////////////print_vector(row_ptr, nrows + 1);
	}
	
	/* Wait for the processors to allocate the necessary memory */
	MPI_Barrier(MPI_COMM_WORLD);	
	
	/* Create the work for each processor */
	rows_of_proc = floor(((rank+1) * nrows)/numtasks) - floor((rank * nrows)/numtasks) ;
	//printf("rows_of_proc for processor %d is %d \n", rank, rows_of_proc);
	
	/* The variable that shows the start of the array 
	that each processor will compute */
	start = floor(((rank) * nrows)/numtasks);
	//printf("rank = %d - start is %d\n", rank, start);	

	/* The variable that shows the end of the array 
	that each processor will compute */
	int end = floor(((rank + 1) * nrows)/numtasks);
	//printf("rank = %d - end is %d\n", rank, end);	
	
	/* Memory allocation for the array results */
	/*results = (int *)malloc(sizeof(int) * rows_of_proc);

	if(results == NULL)
	{
		//Terminate program if there is not enough memory 
		fprintf(stderr, "out of memory \n");
		exit(0);
	}*/	
	
	/* Fill the results array */
	/*for(i = 0; i < rows_of_proc; i++)
		results[i] = 0;	
	*/	
	//fill_with_zeros(results, rows_of_proc);	
	
	/* Wait for the processors to fill the arrays with zeros */
	MPI_Barrier(MPI_COMM_WORLD);	
	//print_vector(results, rows_of_proc);
	
	//if(rank == 0) printf("I am here: rank = %d\n", rank);
	//if(rank == 1) printf("I am here: rank = %d\n", rank);
	//printf("START = %d - RANK = %d\n", start, rank);
	

	while(rounds++ < 100){
	/* Main computation of the result. Each processor
	computes the work that is assigned to it*/
	for(i = start; i < end; i++){
		//printf("rank = %d - i = %d\n", rank, i);
		//printf("RANK = %d\n", rank);
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			y[i] += values[j] * x[col_ind[j]];
			//results[i - start] += values[j] * x[col_ind[j]];
	}
}	
	
	/*for(i = start; i < rows_of_proc; i++){
		printf("rank = %d - i = %d\n", rank, i);
		printf("RANK = %d\n", rank);
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			results[i - start] += values[j] * x[col_ind[j]];
	}*/
	
	MPI_Barrier(MPI_COMM_WORLD);
	//print_vector(results, rows_of_proc);
	////////////if(rank == 0)
		//////////print_vector(y, nrows);
	
	/* The master node receives the sub-results 
	from the other processors and the other nodes
	send their results to the master node*/
	if(rank == 0){
		
		for( i = 1; i < numtasks; i++ )
			MPI_Recv (y + (int)floor((i * nrows)/numtasks), 
			floor(((i+1) * nrows)/numtasks) - floor((i * nrows)/numtasks), MPI_INT, 
			i, tag, MPI_COMM_WORLD, &Stat);
	}	
	else{
			MPI_Send (y + (int)floor((rank * nrows)/numtasks), 
			floor(((rank+1) * nrows)/numtasks) - floor((rank * nrows)/numtasks), MPI_INT, 
			0, tag, MPI_COMM_WORLD);		
	}

	/* Barrier in order to end the time counting */
	MPI_Barrier(MPI_COMM_WORLD);
	end_time = MPI_Wtime();
	
	/* The master node prints the results
	that the processors produced*/
	if(rank == 0){
		//////////////print_vectors(all_results, rows);
		printf("Time elapsed is %1.6f\n", end_time - start_time);
	}
	
	/* End of MPI */
	MPI_Finalize();	
	
	/* Deallocation of memory */	
	free(x);
	free(y);
	free(values);
	free(col_ind);
	free(row_ptr);
	//free(results);	
	
	//Succesful termination
	return 0;
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

	for(i = 0; i < nrows; i++)
		for(j = 0; j < ncolumns; j++)
			array[i][j] = rand() % 2;

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
