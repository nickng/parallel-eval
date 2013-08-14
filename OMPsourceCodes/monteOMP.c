#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int points = 10000000;

/* Function that computes the pi */
double pi_computation(int, int, int);

int main(int argc, char **argv){

	/* Declare start and end of computation time */
	//struct timeval start, end;
	int i;
	/* Declaration of variables to measure the time that has elapsed */
	double start_time;
	double end_time;
	
	/* Thread variables */
	int tid;
	int start;
	int end;
	
	/* Set number of threads */
	int thread_num;
	
	//////////////////////////
	struct timeval tempo1, tempo2;
	
	long elapsed_utime;    /* elapsed time in microseconds */
    long elapsed_mtime;    /* elapsed time in milliseconds */
    long elapsed_seconds;  /* diff between seconds counter */
    long elapsed_useconds; /* diff between microseconds counter */
	
	/* Get the number of threads */
	if(atoi(argv[1]) > 0 && atoi(argv[1]) <= 8)
		thread_num = atoi(argv[1]);
	else {
			printf("Wrong arguments, terminating...\n");	
			exit(0);
		}
	
	/* Set the number of threads */
	omp_set_num_threads( thread_num ) ;	
	
	//gettimeofday(&tempo1, NULL);
	
	/* Number of points */
	//int points;

	/* The final pi and the pi of each processor */
	double pi;
	double current_pi;
	
	/* array that holds the pi that each thread hsa computed */
	double thread_pi[8] = {0};
	
	/* Variable that counts the number of points that are in the circle */
	int inCircle = 0;

	/* The rounds that we will do tha main computation */
	int rounds = 0;
		

	/* Start counting the time */	
	//gettimeofday(&start, NULL);	
	/////////////////////////////////////////////////
	gettimeofday(&tempo1, NULL);
		
/* Do the computation for 10 rounds */		
while(rounds++ < 10){	
	#pragma omp parallel shared ( points )  private( tid, start, end ) 
	{
		/* Get the id of th thread */
		tid = omp_get_thread_num() ;	
	
		/* Start and end of work for each thread */
		start = floor(tid * points/omp_get_num_threads());
		end = floor( (tid + 1) * points/omp_get_num_threads());
	
		/* Call the function that computes the pi */
		thread_pi[tid] = pi_computation(start, end, tid);
	}	
}	
	/* Compute the final pi after the theads finished their computation */
	for(i = 0; i < thread_num; i++ ){
		pi += thread_pi[i];
	}
	pi = pi / thread_num; 
	
	/* Print the pi */
	printf("Final pi is %f\n", pi);
	printf("\n");
	
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
	
	
	/* Termination of main */
	return EXIT_SUCCESS;
}

/* Function that computes the pi */
double pi_computation(int start, int end, int tid){

	/* Variables declaration */
	double pi;
	double x;
	double y;
	int i;
	int inCircle = 0;
	
	/* random seed */
	//srand48( time(0) );
	srand( time(0) + tid );
	
	/* For all the points we compute the number of 
	points that are in the circle
	*/
	for(i = start; i < end; i++){
		/* Compute x and y using the drand48() */
		//x = drand48();
		//y = drand48();	
		x = (double)(rand()%10000)/10000;
		y = (double)(rand()%10000)/10000;		
		
		/* Check if the point is in the circle */
		if( x*x + y*y < 1.0 )
			inCircle ++;
	}	

	/* Computation of current pi for each processor */
	pi = 4 * (double)(inCircle * (tid + 1)) / (double) i;	
	
	//printf("pi in this thread %d is %f\n", tid, pi);
	
	/* return the computed pi */
	return pi ;
}


