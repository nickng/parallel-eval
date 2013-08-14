/*
Parallel computation of Compressed Sparse Row
Algorithm(A*x = y) using SrssionC - Here is the worker1
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libsess.h>
//#include <zmq.h>

#define nrows 1000
#define ncolumns 1000

#define participants 4
#define worker1 1


int main(int argc, char **argv){

	/* Variables declaration */
	int i;
	int j;
	
	/* number of rounds that the algorithm will run */
	int rounds = 0;
	
	/* The number of non zero elements */
	int *num_of_no_zeros = NULL;

	/* Session start */
	session *s;
	join_session(&argc, &argv, &s, "worker1.spr");
	role *Master = s->get_role(s, "Master");	

	/* Receive number of non zero elements */
	receive_int(Master, &num_of_no_zeros);
	
	printf("Num of non zeros received is %d\n", *num_of_no_zeros);
	
	/* Number of non zero elements */
	int no_zeros = *num_of_no_zeros;
	num_of_no_zeros = NULL;
	printf("Num of non zeros NOW is %d\n", no_zeros);
	
	/* Declaration and dynamic memory allocation af array row_ptr */
	int *row_ptr = (int *) malloc( (nrows + 1) * sizeof(int));
	if(row_ptr == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}	
	
	/* Decalration and dynamic memory allocation af array values */
	int *values = (int *) malloc( no_zeros * sizeof(int));
	if(values == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}

	/* Declaration and dynamic memory allocation af col_ind array */
	int *col_ind = (int *) malloc( no_zeros * sizeof(int));
	if(col_ind == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}
	
	/* Declaration and dynamic memory allocation af x array */
	int *x = (int *) malloc( ncolumns * sizeof(int));
	if(x == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}
	
	/* The array that will hold the results of the main computation */
	//int *results = NULL;
	
	/* Define the amount of work that worker 1 will do */
	int start_work;
	int end_work;	
	int amount_of_work;
	
	/* The amount of work that the master will do */
	start_work = floor((worker1 * nrows)/participants) ;
	end_work = floor(((worker1 + 1) * nrows)/participants) ;	
	amount_of_work = (end_work - start_work);
	
	printf("start = %d - end = %d - amount of work = %d\n", start_work, end_work, amount_of_work);	
		
	/* Declaration and dynamic memory allocation af x array */
	int *results = (int *) malloc(2 * amount_of_work * sizeof(int));
	if(results == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}	
	
	//int *numRows = NULL;
	//int *numColumns = NULL;
	
	//int num_of_rows = 1001;
	//int num_of_columns = 1000;
	
	//numRows = &num_of_rows;
	//numColumns = &num_of_columns;
	
	//printf("num_of_rows is %d - num_of_columns is %d\n", *numRows, *numColumns);
	
	/* Expected size of array */
	size_t sz_rows = 1001;
	size_t sz_cols = 1000;
	size_t sz_no_zeros = no_zeros;

	/* Receive row_ptr from master */
	recv_int_array(Master, row_ptr, &sz_rows);
	
	/* Receive values array from master */
	recv_int_array(Master, values, &sz_no_zeros);
	
	/* Receive col_ind array from master */
	recv_int_array(Master, col_ind, &sz_no_zeros);	
	
	/* Receive x array from master */
	recv_int_array(Master, x, &sz_cols);		
	
	//printf("value[%d] = %d\n", 0, values[0]);
	

	
	//print_vector(row_ptr, 1001);
	
/* Run for 100 rounds */	
while(rounds++ < 10000){	
	/* Main computation of the result. Worker1
	computes the work that is assigned to it*/
	for(i = start_work; i < end_work; i++){
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			results[i] += values[j] * x[col_ind[j]];
	}	
}	
	
	//printf("I am here paliiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n");
	printf("Amount of work is %d\n", amount_of_work);
	/* Send the results to the master */
	send_int_array(Master, results, amount_of_work);	
	free(results);
	results = NULL;
	printf("Amount of work is %d\n", amount_of_work);
	
	/* Free memory */
	free(row_ptr);
	free(values);
	free(col_ind);
	free(x);
	//free(results);	
	
	row_ptr = NULL;
	values = NULL;
	col_ind = NULL;
	x = NULL;
	
	
	/* End current session */
	end_session(s);	
	

	
	return EXIT_SUCCESS;
}


