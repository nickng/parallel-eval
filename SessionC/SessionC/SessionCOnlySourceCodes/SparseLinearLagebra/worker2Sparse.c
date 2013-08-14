/*
Parallel computation of Compressed Sparse Row
Algorithm(A*x = y) using SessionC - Here is the worker2
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libsess.h>
//#include <zmq.h>

#define nrows 10000
#define ncolumns 10000

#define participants 4
#define worker2 2

/* Main function */
int main(int argc, char **argv){

	/* Variables declaration */
	int i;
	int j;
	
	/* The number of non zero elements */
	int *num_of_no_zeros = NULL;

	/* number of rounds */
	int rounds = 0;
	
	/* Session start */
	session *s;
	join_session(&argc, &argv, &s, "worker2.spr");
	role *Master = s->get_role(s, "Master");	

	printf("I am here\n");
	
	/* Receive number of non zero elements */
	receive_int(Master, &num_of_no_zeros);
	
	printf("NUm of zeros is %d\n", *num_of_no_zeros);
	
	/* Number of non zero elements */
	int no_zeros = *num_of_no_zeros;	
	
	/* Declaration and dynamic memory allocation af array row_ptr */
	int *row_ptr = (int *) malloc((nrows + 1) * sizeof(int));
	if(row_ptr == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}	
	
	/* Decalration and dynamic memory allocation af array values */
	int *values = (int *) malloc(no_zeros * sizeof(int));
	if(values == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}

	/* Declaration and dynamic memory allocation af col_ind array */
	int *col_ind = (int *) malloc(no_zeros * sizeof(int));
	if(col_ind == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}
	
	/* Declaration and dynamic memory allocation af x array */
	int *x = (int *) malloc(ncolumns * sizeof(int));
	if(x == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}
	
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
	
	printf("value[%d] = %d\n", 0, values[0]);
	
	/* The array that will hold the results of the main computation */
	int *results = NULL;
	
	/* Define the amount of work that worker 1 will do */
	int start_work;
	int end_work;	
	int amount_of_work;
	
	/* The amount of work that the master will do */
	start_work = floor((worker2 * nrows)/participants) / 10;
	end_work = floor(((worker2 + 1) * nrows)/participants) / 10;	
	amount_of_work = (end_work - start_work);
	
	printf("start = %d - end = %d - amount of work = %d\n", start_work, end_work, amount_of_work);	
	
	/* Declaration and dynamic memory allocation af x array */
	results = (int *) malloc(amount_of_work * sizeof(int));
	if(results == NULL){
		fprintf(stderr, "Out of memory, aborting program...\n");
		exit(-1);
	}
	
/* Run for 100 rounds */	
while(rounds++ < 10000){	
	/* Main computation of the result. Worker1
	computes the work that is assigned to it*/
	for(i = start_work; i < end_work; i++){
		for(j = row_ptr[i]; j < row_ptr[i + 1]; j++)
			results[i] += values[j] * x[col_ind[j]];
	}	
}
	
	/* Send the results to the master */
	send_int_array(Master, results, amount_of_work);
	
	/* End current session */
	end_session(s);	
	
	/* Free memory */
	free(values);
	free(col_ind);
	free(x);
	free(results);
	
	return EXIT_SUCCESS;
}