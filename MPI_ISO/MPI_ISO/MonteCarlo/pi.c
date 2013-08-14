#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int main (int argc, char **argv){

	/* Abort after 60 seconds */
	//alarm(60);

	/* Number of rounds */
	int rounds = 0;
	
	/* Number of points */
	int points;
	
	/* Declaration of useful variables */
	int i;
	double x, y; 

	/* The final pi and the pi of each processor */
	double pi;
	double current_pi;

	/* Variable that counts the number of points that are in the circle */
	int inCircle = 0;

	/* Declaration of variables that distribute the work on each processor */
	int start_work;
	int end_work;
	int work_of_each_proc = 0;

	/* Declaration of variables for the MPI */
	int nprocs;
	int rank;

	/* Declaration of variables to measure the time that has elapsed */
	double start_time;
	double end_time;

	/* Struct that declares the current time */
	time_t curtime;

	/*Check if number of points is given
	If it is not given return with an error (abort)
	*/
	if (argc != 2) {
		printf("Number of points have not been given. Aborting program...\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return 1;
	}	
	else	/* Take the number of points */
		points = atoi(argv[1]);
	
	//Initialization of mpi 
	MPI_Init ( &argc, &argv );
	MPI_Comm_size ( MPI_COMM_WORLD, &nprocs );
	MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
	
	/* Barrier in order to start the time counting */
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();	

	/* Define the work that each processor will do */
	start_work = floor( (rank * points) / nprocs );
	end_work = floor( (rank + 1) * points / nprocs ); 
	work_of_each_proc = end_work - start_work;

/* We run it for 12 rounds to take better results */
while(++rounds < 12){

	/* Use of srand48 in order for each processor to 
	achieve different random results by taking into account the rank
	of each processor that is diffrent */
    curtime = time (NULL);
	//srand48(curtime * rank + rand() + rounds);
	srand(curtime * rank + rand() + rounds);
	
	/* For all the points we compute the number of 
	points that are in the circle
	*/
	for(i = start_work; i < end_work; i++){
		/* Compute x and y using the drand48() */
		//x = drand48();
		//y = drand48();
		x = (double)(rand()%10000)/10000;
		y = (double)(rand()%10000)/10000;
	
		/* Check if the point is in the circle */
		if( x*x + y*y < 1.0 )
			inCircle ++;
	}
}//End of while	

	/* Computation of current pi for each processor */
	current_pi = 4 * (double)inCircle / (double)(work_of_each_proc * (rounds - 1));

	/* Use the MPI_Reduce function to compute
	the approximate value of pi (that has been computed from each processor)	
	*/
	MPI_Reduce(&current_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	/* Barrier in order to end the time counting */
	MPI_Barrier(MPI_COMM_WORLD);
	end_time = MPI_Wtime();	

	/* Master process computes the final pi and prints the results */
	if(rank == 0){ 
	
		/*Computation of pi */
		pi = pi / nprocs;
			
		/* Print the results */
		printf("pi is %f \n", pi);
		printf("Time elapsed is %1.6f\n", end_time - start_time);
	}
	
	/* End of mpi */
	MPI_Finalize();
	
	return 0;

}