// Simples Monte Carlo program to calculate pi
// Vitor Vasconcelos - vitors@cdtn.br
// 24/08/2017

// The objective is to use this very simple program
// as a reference to later implement it in OpenCL and
// maybe CUDA

// -------------------------------------------------------
// This version starts to use opencl to make computations
// -------------------------------------------------------

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
#include<string.h> //strcpy()

#include<CL/cl.h>

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

    // Check if OpenCL is present and the devices available
    cl_int err = 0;
    cl_uint n_plat = 0;
    cl_platform_id* id_plat;
    
    // Before checking devices, must check platforms and devices
    // for each platform.
    err = clGetPlatformIDs(0, NULL, &n_plat);
    
    id_plat = malloc(n_plat*sizeof(cl_int));
    err = clGetPlatformIDs(n_plat, id_plat, NULL);

    if(err != CL_SUCCESS)
    {
	fprintf(stderr, "Failed to get number of CL platforms!\n");
    }
    printf("Number of OpenCL platforms found: %d\n", n_plat);

    cl_device_id* device_id;
    size_t string_size;
    cl_uint n_devices;
    char* device_name;
    cl_device_type device_type;
    char str[32] = "";

    // Hold platform number and device number of CPU
    int CPU_plat = -1;
    int CPU_device = -1;
    
    // Loop trough all platforms to find all devices
    i = 0;
    while(i<n_plat)
    {
	// Like platforms, the first call asks the number of devices
	err = clGetDeviceIDs(id_plat[i], CL_DEVICE_TYPE_ALL, 0, NULL, &n_devices);
	if(err != CL_SUCCESS)
	{
	    fprintf(stderr, "Failed to get the number of devices!\n");
	}
	device_id = malloc(n_devices*sizeof(cl_device_id));

	uint j=0;
	while(j<n_devices)
	{
	    // make string empty
	    err = clGetDeviceIDs(id_plat[i], CL_DEVICE_TYPE_ALL, 1, &device_id[i], NULL);
	    if(err != CL_SUCCESS)
	    {
		fprintf(stderr, "Failed to get device ID!\n");
	    }
	    // ----------------------------
	    // Must have a loop for devices
	    // ----------------------------
	    
	    // Get and print device information
	    err = clGetDeviceInfo(device_id[i], CL_DEVICE_NAME, 0, NULL, &string_size);
	    if(err != CL_SUCCESS)
	    {
		fprintf(stderr, "Failed to get device INFO!\n");
	    }
	    // Allocate a string to store device's name
	    device_name = malloc(string_size*sizeof(char));
	    err = clGetDeviceInfo(device_id[i], CL_DEVICE_NAME, string_size, device_name, NULL);
	    printf("Found the following device(s) for platform %ld:\n --- Device name: %s\n", i, device_name);
	    free(device_name);
	
	    // ------------------------------------------------------------------------------------
	    // Get device type information
	    //
	    // Important: cl_device_type is an enumeration set. The only way to print the type
	    // is coding the list inside the function
	    // One way is to compare bitwase: if (myargument & CL_DEVICE_TYPE_GPU)
	    err = clGetDeviceInfo(device_id[i], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
	    if(err != CL_SUCCESS)
	    {
		fprintf(stderr, "Failed to get device INFO!\n");
	    }
	    if(device_type & CL_DEVICE_TYPE_GPU) strcpy(str,"GPU");
	    else if(device_type & CL_DEVICE_TYPE_CPU) {
		strcpy(str,"CPU");
		CPU_plat = i;
		CPU_device = j;
	    }
	    else if(device_type & CL_DEVICE_TYPE_ACCELERATOR) strcpy(str,"ACCELERATOR");
	    else if(device_type & CL_DEVICE_TYPE_DEFAULT) strcpy(str,"DEFAULT");
	    else strcpy(str, "---");
	
	    printf(" --- Device type: %s\n\n", str);
	    j++;
	}
	i++;
    }
    free(id_plat);

    // With all platforms and devices, create a context for the CPU
    printf("My chosen platform is %d and chosen device is %d\n", CPU_plat, CPU_device);
    
    cl_context my_context;
    my_context = clCreateContext(NULL, 1, &device_id[CPU_device], NULL, NULL, &err);
    if(err != CL_SUCCESS)
    {
	fprintf(stderr, "Failed to get CONTEXT!\n");
	return(errno);
    }
    
    size_t deviceBufferSize = -1;
    err = clGetContextInfo(my_context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    if(err != CL_SUCCESS)
    {
	fprintf(stderr, "Failed to get CONTEXT info!\n");
	return(errno);
    }
    err = clGetContextInfo(my_context, CL_CONTEXT_DEVICES, deviceBufferSize, &device_id[CPU_device], NULL);

    printf("---------------------------------------------------------------- \n\n");
    // If find a GPU, give preference to it.
    // Othewise, run in parallel in a CPU
    
    
    // Play with the following elements of openCL:
    // - cl_devide_id
    // - cl_kernel
    // - cl_program
    // - cl_command_queue
    // - cl_context
    
    // Check if the argument is ok
    if (argc < 2) {
	fprintf(stderr, "Wrong number of arguments. Using the default: 100000 numbers.\n");
	num = 100000;
    }
    else
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

    free(device_id);
    
    return 0;
}
