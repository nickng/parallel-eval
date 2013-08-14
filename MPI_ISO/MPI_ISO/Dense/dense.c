#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

//Declare heigth and width of the arrays
#define rows 10000
#define columns 10000

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


	/* Declaration of variables to measure the time that has elapsed */
	double start_time;
	double end_time;

	//Declare variables
	int i = 0;
	int j = 0;
	int k = 0;
	int rows_of_proc;
	int start;
	int end;
	
	//Mpi variables
	int rank;
	int numtasks;
	int tag = 1;
	
	/* Number of rounds */
	int rounds = 0;
	
	//Declaration of arrays
	int b[4][4] = {{1,2,3,4}, {2,3,4,5}, {6,7,8,9}, {0,1,2,3}};
	int c[4] = {1,2,3,4};
	
	//Dynamic memory allocation of array C
	int *C;
	C = (int *) malloc(columns * sizeof(int *));
	
	if(C == NULL)
	{
		//Terminate program if there is not enough memory 
		fprintf(stderr, "out of memory \n");
		exit(0);
	}
	
	//Fill matrix C with random values
	fill_vector(C, columns);
	//printf("Printing vector C\n");
	/////////////////////////print_vectors(C, columns);
	//printf("\n");	
	
	//Dynamic memory allocation of array B
	int **B;
	B = (int **) malloc(rows * sizeof(int *));
	
	if(B == NULL)
	{
		//Terminate program if there is not enough memory 
		fprintf(stderr, "out of memory \n");
		exit(0);
	}
	
	for(i = 0; i < rows; i++){
		B[i] = (int *) malloc(columns * sizeof(int));
		if(B[i] == NULL)	
		{
			//Terminate program if there is not enough memory 
			fprintf(stderr, "out of memory \n");
			exit(0);
		}
	}

	//Fill matrix B with random values
	fill_matrix(B);
	//printf("Printing matrix B\n");
	//print_matrix(B);
	//printf("\n");
	
	/* The results of the computation */
	int *all_results;
	all_results = (int *)malloc( rows * sizeof(int) );
	/* Check if we have enough memory */
	if(all_results == NULL)
	{
		//Terminate program if there is not enough memory 
		fprintf(stderr, "out of memory \n");
		exit(0);
	}	
	
	//Mpi initialization
	MPI_Status Stat;
	MPI_Init ( &argc, &argv );
	MPI_Comm_size ( MPI_COMM_WORLD, &numtasks );
	MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
	/* Barrier in order to start the time counting */
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();
	
	/* Create the sub-arrays for each processor */
	rows_of_proc = floor(((rank+1) * rows)/numtasks) - floor((rank * rows)/numtasks) ;
	/* The start of each processor's array computation
	(the work that wiil be assigned to it) */
	start = floor((rank * rows)/numtasks);
	/* The end of each processor's computation
	(the work that wiil be assigned to it) */
	end = floor(((rank + 1) * rows)/numtasks);
	
	//printf("rows_of_proc for processor %d is %d \n", rank, rows_of_proc);
	//printf("rank is %d - start is %d\n", rank, start);
	//printf("rank is %d - end is %d\n", rank, end);
	
	//Wait until all start - end have been computed
	MPI_Barrier(MPI_COMM_WORLD);
	
	/* The sub - results will be gathered in this array */
	/*int *results;
	results = (int *)malloc(sizeof(int) * rows_of_proc);

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

	/* Fill the results array */
	//for(i = 0; i < rows; i++)
		//all_results[i] = 0;	
	fill_with_zeros(all_results, rows);
	//for(i = 0; i < rows; i++)
		//printf("all_results[%d] = %d\n", i, all_results[i]);
		
	/* Barrier */	
	MPI_Barrier (MPI_COMM_WORLD);
	
	while(rounds++ < 50){
		/* Each processor computes it's own part of the array */
		for(i = start; i < end; i++)
			for(j = 0; j < columns; j++){
				all_results[i] += B[i][j] * C[j];
			}
	}
	
	//Barrier to print the results
	//MPI_Barrier (MPI_COMM_WORLD);
	///////////////////print_vectors(all_results, rows);
	MPI_Barrier (MPI_COMM_WORLD);
	
	/* The master node receives the sub-results 
	from the other processors and the other nodes
	send their results to the master node*/
	if(rank == 0){
		
		for( i = 1; i < numtasks; i++ )
			MPI_Recv (all_results + (int)floor((i * rows)/numtasks), 
			(int)floor(((i+1) * rows)/numtasks) - (int)floor((i * rows)/numtasks), MPI_INT, 
			i, tag, MPI_COMM_WORLD, &Stat);
	}	
	else{
			MPI_Send (all_results + (int)floor((rank * rows)/numtasks), 
			(int)floor(((rank+1) * rows)/numtasks) - (int)floor((rank * rows)/numtasks), MPI_INT, 
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
		printf("rounds = %d\n", rounds);
	}
	
	MPI_Barrier (MPI_COMM_WORLD);
	//Free memory
	for(i = 0; i < rows; i++ )
		free(B[i]);
	free(B);
	free(C);	
	//free(results);
	free(all_results);
	
	/* End of Mpi */
	MPI_Finalize();
	
	return 0;
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

//Fill the matrix with random values
//void fill_matrix(int array[rows][columns]){
void fill_matrix(int **array){

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
	
	//Variable declaration
	int i;

	for(i = 0; i < length; i++)
		vector[i] = 0;

}
