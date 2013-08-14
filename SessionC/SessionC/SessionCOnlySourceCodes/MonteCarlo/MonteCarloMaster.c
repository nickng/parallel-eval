#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>

#include <libsess.h>

#define participants 4

/* Function that computes the pi */
double pi_computation(int);

int main(int argc, char **argv){

	/* Declare start and end of computation time */
	struct timeval start, end;
	
	/* Number of points */
	int points;

	/* The final pi and the pi of each processor */
	double pi;
	double current_pi;

	/* The pi that the master wiil receive from the slaves */
	double pi_from_slave1 = 0.0;
	double pi_from_slave2 = 0.0;
	double pi_from_slave3 = 0.0;
	
	/* Variable that counts the number of points that are in the circle */
	int inCircle = 0;

	/* The rounds that we will do tha main computation */
	int rounds = 0;
	
	/* sessionC handler */
	session *s;
	join_session(&argc, &argv, &s, "MonteCarlo_Master.spr");
	role *Slave1 = s->get_role(s, "Slave1");
	role *Slave2 = s->get_role(s, "Slave2");	
	role *Slave3 = s->get_role(s, "Slave3");
	
	/*Check if number of points is given or if the number of points is greater than zero
	If it is not given return with an error
	*/
	if (argv[0] <= 0) {
		printf("Number of points have not been given, or is an invalid number. Aborting program...\n");
		exit(0);
	}	
	else	/* Take the number of points */
		points = atoi(argv[0]);

	/* Start counting the time */	
	gettimeofday(&start, NULL);	
		
/* Do the computation for 1000 rounds */		
while(rounds++ < 20){	
	/* Call the function that computes the pi */
	current_pi = pi_computation(points);
}	
	
	/* Master's pi */
	printf("pi of master is %f\n", current_pi);
	
	/* receive pi from slave1 */
	recv_double(Slave1, &pi_from_slave1);
	recv_double(Slave2, &pi_from_slave2);	
	recv_double(Slave3, &pi_from_slave3);
	
	/* Computation of Final pi */
	pi = (double)(current_pi + pi_from_slave1 + pi_from_slave2 + pi_from_slave3) / participants;
	
	/* Print the pi */
	printf("Final pi is %f\n", pi);
	
	/* End counting of time here */
	gettimeofday(&end, NULL);
	
	long time_elapsed = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000000;
	 
	/* Print the time that has elapsed */
	printf("Elapsed time is: %ld seconds\n", time_elapsed);
	
	/* End current session */
	end_session(s);
	
	/* Termination of main */
	return EXIT_SUCCESS;
}

/* Function that computes the pi */
double pi_computation(int points){

	/* Variables declaration */
	double pi;
	double x;
	double y;
	int i;
	int inCircle = 0;
	
	/* random seed */
	srand48( time(0) );
	
	/* For all the points we compute the number of 
	points that are in the circle
	*/
	for(i = 0; i < points; i++){
		/* Compute x and y using the drand48() */
		x = drand48();
		y = drand48();	
		
		/* Check if the point is in the circle */
		if( x*x + y*y < 1.0 )
			inCircle ++;
	}	

	/* Computation of current pi for each processor */
	pi = 4 * (double)inCircle / (double) i;	
	
	/* return the computed pi */
	return pi;
}
