// Simples Monte Carlo program to calculate pi
// Vitor Vasconcelos - vitors@cdtn.br
// 23/08/2017

// The objective is to use this very simple program
// as a reference to later implement it in OpenCL and
// maybe CUDA

// This program uses the equation of the circle to
// check if points randomly* chosen to be inside
// a 1x1 square belongs to the inscribled circle
// of diameter 1
//
// The proportion of points inside the circle
// above the total number of points gives pi/4

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<math.h>

// Using double drand48(void) to generate
// random numbers (altough it's being called in a way
// pseudo numbers are generated

int main(int argc , char* argv[])
{
    long unsigned int num;
    long unsigned int i = 0;
    
    float x = 0.0;
    float y = 0.0;
    float point = 0.5;
    float r = 0.0;
    
    long unsigned int total = 0;

    double pi = 0.0;
    
    // Check if the argument is ok
    if (argc < 2) {
	fprintf(stderr, "Wrong number of arguments (<2). Quiting...\n");
	return(errno);
    }
    
    // Convert the argument to integer
    num = atoi(argv[1]);

    while(i<num)
    {
	x = drand48();
	y = drand48();

	// Check if point belongs to circle
	r = sqrt(pow((x-point),2)+pow((y-point),2));

	// Get the points inside the circle
	if (r<point)
	    total++;
	pi=4*(double)total/num;

	// avoid infinity loop
	i++;
    }

    printf("For %ld tries pi is %.6f\n", num, pi);
    
    return 0;
}
