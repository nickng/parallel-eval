#include <stdio.h>
#include<unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include "mpi.h"

//Contains the struct to compute the time
struct timeval stime, endtime;

int main (int argc, char **argv){

	//Abort after 60 seconds
	//alarm(60);
	
	//Number of points
	int points;
	
	//Declaration of variables
	double x, y; 
	double pi;
	double current_pi;
	int nprocs;
	int rank;
	int inCircle = 0;
	int start_time;
	int end_time;
	int i;
	
	/*Check if number of points is given
	If it is not given return with an error
	*/
	if (argc != 2) {
		MPI_Abort(MPI_COMM_WORLD, 1);
		return 1;
	}	
	else
		points = atoi(argv[1]);

	//printf("points = %d \n", points);
	
	//Initialization of mpi 
	MPI_Init ( &argc, &argv );
	MPI_Comm_size ( MPI_COMM_WORLD, &nprocs );
	MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	/* If process 0 then start the clock */
	if(rank == 0){
		printf("Monde Carlo for the computation of pi is starting \n");
		//MPI_Barrier(MPI_COMM_WORLD);
		start_time = MPI_Wtime();
		printf("start time is %d \n", start_time);
	}
		//MPI_Barrier(MPI_COMM_WORLD);
		//start_time = MPI_Wtime();
		
	/* Use the gettimeofday to gauge the time */
	gettimeofday(&stime, NULL);
	srand( time(NULL) + (rank + 1) * 600 );
	
	/*For all the points we compute the number of 
	points that are in the circle
	*/
	for(i = 0; i < points; i++){
		//Compute x and y
		x = (double)(rand()%10000)/10000;
		y = (double)(rand()%10000)/10000;
	
		//Check if the point is in the circle
		if( x*x + y*y <= 1.0 )
			inCircle ++;
	}
	
	/* Computation of current pi for each processor */
	current_pi = 4 * (double)inCircle / (double)points;
	
	//printf("current_pi = %f \n",current_pi);
	
	/*Use the mpi Reduce function to compute
	the approximate value of pi (that has been computed to each processor)	
	*/
	MPI_Reduce(&current_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if(rank == 0){ //printf("Sum = %f \n", pi);
	
		/*Computation of pi */
		pi = pi / nprocs;
	
		/* Total time elapsed */
		end_time = MPI_Wtime();
		
		printf("end time is %d \n", end_time);
		
		gettimeofday(&endtime, NULL);
		
		printf("time now is %10.8f\n", ((endtime.tv_sec * 1000000 + endtime.tv_usec)
		  - (stime.tv_sec * 1000000 + stime.tv_usec)));
		
		/* Print the results */
		printf("pi is %f \n", pi);
		printf("Time elapsed is %10.8f", end_time - start_time);
	}
	//printf("x = %f \n",(double)(rand()%10000)/10000);
	
	//End of mpi
	MPI_Finalize();
	
	return 0;

}


