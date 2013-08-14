/*
Parallel computation of Compressed Sparse Row
Algorithm(A*x = y) using OpenMP
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

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

/* Work in parallel */
void work(int, int);

/* Declaration of array x, which is the vector 
that will bw multiplied with the array */
int *x = NULL;

/* The array that contains the start of each row within
the non-zero elements array*/
int *row_ptr = NULL;

/* The array that will hold the non zero values */
int *values = NULL;
	
/* The array that will hold the indexes of the columns */
int *col_ind = NULL;

/* Declaration of array y, which is the vector 
that will hold the results */
int *y = NULL;

//Declaration of array A
int **A = NULL;

int main(int argc ,char **argv){

	//////////////////////////
	struct timeval tempo1, tempo2;
	
	long elapsed_utime;    /* elapsed time in microseconds */
    long elapsed_mtime;    /* elapsed time in milliseconds */
    long elapsed_seconds;  /* diff between seconds counter */
    long elapsed_useconds; /* diff between microseconds counter */

	/* Variables declaration */
	int i = 0;
	int j = 0;
	int non_zero_elements = 0;
	
	/* Number of threads */
	int thread_num = 2;
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
	
	//printf("num of threads is %d \n", omp_get_num_threads());
	
	
	/* Define number of rounds */
	int rounds = 0;
	
	/* Start counting time */
	gettimeofday(&tempo1, NULL);
	

	//Dynamic memory allocation
	x = (int *) malloc( ncolumns * sizeof(int) );
	if(x == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}
	
	//Fill the vector x
	//printf("Filling and printing vector x \n");
	fill_vector(x, ncolumns);
	////////////////////////////////////////////////print_vector(x, ncolumns);	
	//printf("\n");	
	

	
	/* Dynamic memory allocation of the array row_ptr */
	row_ptr = (int *) malloc( (nrows + 1) * sizeof(int) );
	/* Check if we have enough memory */
	if(row_ptr == NULL)
	{
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}

	
	/* Dynamic memory allocation */
	y = (int *) malloc( nrows * sizeof(int) );
	fill_with_zeros(y, nrows);	
			
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
	
	
//printf("y[0] is %d\n", y[0]);
	
while(rounds++ < 100){	
	  
	#pragma omp parallel shared( row_ptr, y, values, col_ind ) private( tid, start, end ) 
	{
	
		/* Get the id of th thread */
		tid = omp_get_thread_num() ;	
	
		start = floor(tid * nrows/omp_get_num_threads());
		end = floor( (tid + 1) * nrows/omp_get_num_threads());
		
		//printf("start = %d - end = %d - tid = %d \n", start, end, tid);
		
		/* Parallel execution */
		work(start, end);
		
	}
}//end of while	
	
	//printf("y[0] is %d\n", y[0]);
	
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
	free(x);
	x = NULL;
	free(y);
	y = NULL;
	free(values);
	values = NULL;
	free(col_ind);
	col_ind = NULL;
	free(row_ptr);
	row_ptr = NULL;
	
	/* Exit success */
	return 0;
}

/* Main computation */
void work(int start, int end){

		int i,j;

		for(i = start; i < end; i++){
			for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
				y[i] += values[j] * x[col_ind[j]];
			//results[i - start] += values[j] * x[col_ind[j]];
		}


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