#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

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

/* Main computation */
void computation (int, int);

/* Declaratoin of the arrays */
int *results = NULL;
int *C = NULL;
int **B = NULL;
	
int main(int argc, char **argv){


	//////////////////////////
	struct timeval tempo1, tempo2;
	
	long elapsed_utime;    /* elapsed time in microseconds */
    long elapsed_mtime;    /* elapsed time in milliseconds */
    long elapsed_seconds;  /* diff between seconds counter */
    long elapsed_useconds; /* diff between microseconds counter */	
	

	//Declare variables
	int i = 0;
	int j = 0;
	int k = 0;
	
	/* Thread variables */
	int tid;
	int start;
	int end;	
	
	/* NUmber of threads */
	int thread_num;
	
	/* Number of rounds */
	int rounds = 0;
	
	/* Get the number of threads */
	if(atoi(argv[1]) > 0 && atoi(argv[1]) <= 8)
		thread_num = atoi(argv[1]);
	else {
			printf("Wrong arguments, terminating...\n");	
			exit(0);
		}

	/* Set the number of threads */
	omp_set_num_threads( thread_num ) ;	
	
	//Dynamic memory allocation of array C
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
	

	results = (int *)malloc( rows * sizeof(int) );
	/* Check if we have enough memory */
	if(results == NULL)
	{
		//Terminate program if there is not enough memory 
		fprintf(stderr, "out of memory \n");
		exit(0);
	}	
	
	/* Start counting the time */	
	gettimeofday(&tempo1, NULL);	
	
while(rounds++ < 50){
	#pragma omp parallel shared ( results, C, B )  private( tid, start, end ) 
	{	
		/* Get the id of th thread */
		tid = omp_get_thread_num() ;
		
		/* Define the work that each thread will perform */
		start = floor(tid * rows / omp_get_num_threads());
		end = floor((tid + 1) * rows / omp_get_num_threads());
	
		//printf("start = %d - end = %d - tid = %d \n", start, end, tid);

		/* Each processor computes it's own part of the array */
		computation(start, end);
	}		
}// end of while	
	
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
	
	//Free memory
	for(i = 0; i < rows; i++ )
		free(B[i]);
	free(B);
	free(C);	
	free(results);
	
	return 0;
}

/* Main computation */
void computation (int start, int end){

	/* Variables declaration */
	int i,j;

	/* Each processor computes it's own part of the array */
	for(i = start; i < end; i++)
		for(j = 0; j < columns; j++){
			results[i] += B[i][j] * C[j];
		}
		
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
	
		
	