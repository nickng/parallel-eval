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

	/* Number of points */
	int points;

	/* The pi of the slave */
	double pi;
	
	/* Variable that counts the number of points that are in the circle */
	int inCircle = 0;

	/* The rounds that we will do tha main computation */
	int rounds = 0;	
	
	/* sessionC handler */
	session *s;
	join_session(&argc, &argv, &s, "MonteCarlo_Slave1.spr");
	role *Master = s->get_role(s, "Master");	
  
	/*Check if number of points is given or if the number of points is greater than zero
	If it is not given return with an error
	*/
	if (argv[0] <= 0) {
		printf("Number of points have not been given, or is an invalid number. Aborting program...\n");
		exit(0);
	}	
	else	/* Take the number of points */
		points = atoi(argv[0]);
	
while(rounds++ < 20){	
	/* Call the function that computes the pi */
	pi = pi_computation(points);
}	
	
	printf("pi of Slave1 is %f\n", pi);

	/* send the computed pi to the master */
	send_double(Master, pi);
	
	/* End current session */
	end_session(s);	
	
	/* Successful termination of main */
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
	
	
	