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
# define SIZE 16 // Achei o erro! Estava no clEnqueueNDRangeKernel
                // O parâmetro que estava sendo passado estava como SIZE
                // e não apenas a dimensao dos meus dados. Mudei para 1, e ok.

// The proportion of points inside the circle
// above the total number of points gives pi/4

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<math.h>
#include<string.h> //strcpy()

#include<CL/cl.h>

// This file is generated trought the python2 script clext.py
// This script is made available py WanghongLin at github.
// ALL RIGHTS RESERVED TO THE AUTHOR
#include"clext.h"

// Typedefs suggested by AJ Guillon to avoid little problems
//typedef char             int8_t;
typedef unsigned char   uint8_t;
typedef short           int16_t;
typedef unsigned short uint16_t;
typedef int             int32_t;
typedef unsigned int   uint32_t;
typedef long            int64_t;
typedef unsigned long  uint64_t;

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
    if(err != CL_SUCCESS)
    {
      fprintf(stderr, "Failed to get number of CL platforms!\n");
    }
    
    id_plat = malloc(n_plat*sizeof(cl_int));
    err = clGetPlatformIDs(n_plat, id_plat, NULL);
    if(err != CL_SUCCESS)
    {
      printf(" ---- CL Error: %s\n", clGetErrorString(err));
    }

    printf("Number of OpenCL platforms found: %d\n\n", n_plat);
    for(int z=0; z<n_plat; z++)
    {
      char strname[128]="";
      clGetPlatformInfo(id_plat[z], CL_PLATFORM_NAME, 128*sizeof(char), strname, NULL);
      printf(" --- platform %d is %s\n", z, strname);
    }
    printf("\n");
    
    cl_device_id device_id;
    size_t string_size;
    cl_uint n_devices;
    char* device_name;
    cl_device_type device_type;
    char str[32] = "";

    // Hold platform number and device number of CPU
    int CPU_plat = -1;
    int CPU_device = -1;
    int GPU_plat = -1;
    int GPU_device = -1;
    
    // Loop trough all platforms to find all devices
    i = 0;
    int outer_n_plat = n_plat;
    while(i<outer_n_plat)
    {
      printf(" ----------- n_plat= %d\n\n", n_plat);
      printf(" ----------- outer_n_plat= %d\n\n", outer_n_plat);
      
      // Like platforms, the first call asks the number of devices
      err = clGetDeviceIDs(id_plat[i], CL_DEVICE_TYPE_ALL, 0, NULL, &n_devices);

      if(err != CL_SUCCESS)
	{
	  fprintf(stderr, "Failed to get the number of devices!\n");
	}
      printf("Platform %ld has %d devices.\n", i, n_devices);
      
      uint j=0;
      while(j<n_devices)
	{
	  // make string empty
	  err = clGetDeviceIDs(id_plat[i], CL_DEVICE_TYPE_ALL, n_devices, &device_id, NULL);
	  if(err != CL_SUCCESS)
	    {
	      fprintf(stderr, "Failed to get device ID1!\n");
	    }
	  // ----------------------------
	  // Must have a loop for devices
	  // ----------------------------
	  
	  // Get and print device information
	  err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &string_size);
	  if(err != CL_SUCCESS)
	    {
	      fprintf(stderr, "Failed to get device INFO!\n");
	    }
	  // Allocate a string to store device's name
	  device_name = malloc(string_size*sizeof(char));
	  err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, string_size, device_name, NULL);
	  printf("Found the following device(s) for platform %ld:\n --- Device name: %s\n", i, device_name);
	  free(device_name);
	
	  // ------------------------------------------------------------------------------------
	  // Get device type information
	  //
	  // Important: cl_device_type is an enumeration set. The only way to print the type
	  // is coding the list inside the function
	  // One way is to compare bitwise: if (myargument & CL_DEVICE_TYPE_GPU)
	  err = clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
	  if(err != CL_SUCCESS)
	    {
	      fprintf(stderr, "Failed to get device INFO!\n");
	    }
	  if(device_type & CL_DEVICE_TYPE_GPU) {
	    strcpy(str,"GPU");
	    GPU_plat = i;
	    GPU_device = j;
	  }
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
    //    free(id_plat);

    // Give a new device to avoid using the stored one in case of problem calling
    // the clGetDeviceIds function. This must be corrected later.
    cl_device_id new_device;
    
    // When I know who is who, get the device I want
    // OBS: intermitent error in CAPRARA when using GPU. Sometimes I get SEGFAULT,
    // sometimes it runs (without filling my data vector).
    err = clGetDeviceIDs(id_plat[GPU_plat], CL_DEVICE_TYPE_ALL, 1, &new_device, NULL);

    if(err != CL_SUCCESS)
      {
	printf(" ---- CL Error: %s\n", clGetErrorString(err));
	exit(0);
      }
    
    
    cl_context my_context;
    my_context = clCreateContext(NULL, 1, &new_device, NULL, NULL, &err);
    if(err != CL_SUCCESS)
    {
	printf(" ---- CL Error: %s\n", clGetErrorString(err));
	return(errno);
    }
    
    size_t deviceBufferSize = -1;
    err = clGetContextInfo(my_context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    if(err != CL_SUCCESS)
    {
	printf(" ---- CL Error: %s\n", clGetErrorString(err));
	return(errno);
    }
    printf("\n --- deviceBufferSize is: %ld\n", deviceBufferSize);
    err = clGetContextInfo(my_context, CL_CONTEXT_DEVICES, deviceBufferSize, &device_id, NULL);
    
    if(err != CL_SUCCESS)
    {
	fprintf(stderr, "Failed to get CONTEXT info!\n");
	return(errno);
    }

    // Must create a clCommandQueue
    cl_command_queue my_queue;
    //  my_queue = clCreateCommandQueue(my_context, device_id[CPU_device], 0, NULL); Deprecated!
    my_queue = clCreateCommandQueueWithProperties(my_context, device_id, 0, NULL);
    if(err != CL_SUCCESS)
    {
	fprintf(stderr, "Failed to create CL COMMAND QUEUE!\n");
	return(errno);
    }

    // After the command queue, how to execute my kernel?
    // This a dummy kernel only to test compilation and execution
    // Only one not used argument
    char * source = {
	"__kernel void mc(__global int *data)\n"
	"{\n"
	"int id = get_global_id(0);\n"
	"data[id] = id;\n"
        "}\n"
    };
    cl_program my_program;
    my_program = clCreateProgramWithSource(my_context, 1, (const char**)&source, NULL, NULL);
    clBuildProgram(my_program, 0, NULL, NULL, NULL, NULL);

    cl_kernel my_kernel;
    my_kernel = clCreateKernel(my_program, "mc", NULL);
    
    cl_int data[SIZE];
    for(int c=0; c<SIZE; c++)
	data[c]=-1;

    printf("\n --- BEFORE: ");
    for(int c=0; c<SIZE; c++)
	printf("%d ", data[c]);
    printf("\n");
    
    // Create a buffer with data to my kernel (I'm not using it to write, only to get
    // the values of the kernel)
    cl_mem my_buffer;
    my_buffer = clCreateBuffer(my_context, CL_MEM_READ_ONLY, SIZE*sizeof(cl_int), NULL, NULL); 

    // Enqueue and execute the kernel
    clEnqueueWriteBuffer(my_queue, my_buffer, CL_FALSE, 0, SIZE*sizeof(cl_int), &data, 0, NULL, NULL);
    clSetKernelArg(my_kernel, 0, sizeof(my_buffer), &my_buffer);

    size_t global_dim[] = {SIZE, 0, 0}; // Quantas dimensoes? 
    clEnqueueNDRangeKernel(my_queue, my_kernel, 1, NULL, global_dim, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(my_queue, my_buffer, CL_FALSE, 0, SIZE*sizeof(cl_int), &data, 0, NULL, NULL);

    clFinish(my_queue);

    printf("\n --- AFTER: ");
    for(int c=0; c<SIZE; c++)
	printf("%d ", data[c]);
    printf("\n");
    
    clReleaseCommandQueue(my_queue);
    clReleaseContext(my_context);

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

	// Would be a nice feature to calculate
	// the variance of pi calculations
    }

    printf("For %ld tries pi is %.6f\n", num, pi);

    return 0;
}
