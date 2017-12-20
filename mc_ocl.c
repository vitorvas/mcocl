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
//# define SIZE (long int)691000 // Achei o erro! Estava no clEnqueueNDRangeKernel
# define SIZE (long int)8192                // O parâmetro que estava sendo passado estava como SIZE
                // e não apenas a dimensao dos meus dados. Mudei para 1, e ok.
//# define SIZE (long int)32

// The proportion of points inside the circle
// above the total number of points gives pi/4

#include<stdio.h>
#define _XOPEN_SOURCE    /* ALWAYS BEFORE the include statement: needed by drand48() */ 
#include<stdlib.h>
#include<errno.h>
#include<math.h>
#include<string.h>   // strcpy()
#include<limits.h>   // Try to use the maximum integer number to calculate pi
#include<sys/stat.h> // Para usar as estruturas de stat
#include<time.h>     // Measure the time of execution

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
// pseudo numbers are generated)

int main(int argc , char* argv[])
{
  // To measure execution time
    clock_t t;
  
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
      // Like platforms, the first call asks the number of devices
      err = clGetDeviceIDs(id_plat[i], CL_DEVICE_TYPE_ALL, 0, NULL, &n_devices);

      if(err != CL_SUCCESS)
	{
	  fprintf(stderr, "Failed to get the number of devices!\n");
	}
      printf("Platform %ld has %d devices.\n", i, n_devices);
      
      cl_device_id devid[n_devices];
      
      // make string empty
      err = clGetDeviceIDs(id_plat[i], CL_DEVICE_TYPE_ALL, n_devices, devid, NULL);
      
      if(err != CL_SUCCESS)
	{
	  printf(" ---- CL Error: %s\n", clGetErrorString(err));
	  exit(0);
	}
      
      unsigned int j=0;
      while(j<n_devices)
      {
	// ----------------------------
	// Must have a loop for devices
	// ----------------------------
	
	// Get and print device information
	err = clGetDeviceInfo(devid[j], CL_DEVICE_NAME, 0, NULL, &string_size);
	//	  printf("String size of device name is %d.\n", string_size);
	if(err != CL_SUCCESS)
	{
	  printf(" ---- CL Error: %s\n", clGetErrorString(err));
	  exit(0);
	}
	// Allocate a string to store device's name
	device_name = malloc(string_size*sizeof(char));
	err = clGetDeviceInfo(devid[j], CL_DEVICE_NAME, string_size, device_name, NULL);
	printf("Found device %d for platform %ld:\n --- Device name: %s\n", j, i, device_name);
	strcpy(device_name, "");
	free(device_name);
	
	
	
	// ------------------------------------------------------------------------------------
	// Get device type information
	//
	// Important: cl_device_type is an enumeration set. The only way to print the type
	// is coding the list inside the function
	// One way is to compare bitwise: if (myargument & CL_DEVICE_TYPE_GPU)
	err = clGetDeviceInfo(devid[j], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
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
    
    // Give a new device to avoid using the stored one in case of problem calling
    // the clGetDeviceIds function. This must be corrected later.
    cl_device_id new_device[2];
    
    // When I know who is who, get the device I want
    // OBS: intermitent error in CAPRARA when using GPU. Sometimes I get SEGFAULT,
    // sometimes it runs (without filling my data vector).
    // err = clGetDeviceIDs(id_plat[GPU_plat], CL_DEVICE_TYPE_GPU, 1, new_device, NULL);
    err = clGetDeviceIDs(id_plat[CPU_plat], CL_DEVICE_TYPE_CPU, 1, new_device, NULL);

    if(err != CL_SUCCESS)
      {
	printf(" ---- CL Error: %s\n", clGetErrorString(err));
	exit(0);
      }
    
    
    cl_context my_context;
    my_context = clCreateContext(NULL, 1, &new_device[0], NULL, NULL, &err);
    if(err != CL_SUCCESS)
    {
	printf(" ---- CL Error: %s\n", clGetErrorString(err));
	return(errno);
    }
    
    size_t deviceBufferSize = -1;
    err = clGetContextInfo(my_context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    // Asking for the number of devices, which returns a cl_uint, i.e., 8 bytes.
    if(err != CL_SUCCESS)
    {
	printf(" ---- CL Error: %s\n", clGetErrorString(err));
	return(errno);
    }

    err = clGetContextInfo(my_context, CL_CONTEXT_DEVICES, deviceBufferSize, &device_id, NULL);

    if(err != CL_SUCCESS)
    {
        printf(" ---- CL Error: %s\n", clGetErrorString(err));
	return(errno);
    }

    // Must create a clCommandQueue
    cl_command_queue my_queue;
    //  my_queue = clCreateCommandQueue(my_context, device_id[CPU_device], 0, NULL); Deprecated!
    my_queue = clCreateCommandQueueWithProperties(my_context, device_id, 0, NULL);
    if(err != CL_SUCCESS)
    {
        printf(" ---- CL Error: %s\n", clGetErrorString(err));
	return(errno);
    }
    
    // Create the compute program from the kernel source file
    cl_program my_program;
    char *fileName = "kernels.cl";
    FILE *fh = fopen(fileName, "r");
    if(!fh) {
        printf("Error: Failed to open file containing kernels. EXITING...\n");
        return(errno);
    }
    // stat is a system call to get file status
    // man stat(2)
    struct stat statbuf;
    stat(fileName, &statbuf);

    char *kernel = (char *) malloc(statbuf.st_size + 1);
    fread(kernel, statbuf.st_size, 1, fh);
    kernel[statbuf.st_size] = '\0';

    // Kernels are now on the file kernels.cl
    my_program = clCreateProgramWithSource(my_context, 1, (const char**)&kernel, NULL, NULL);

    const char opt[]="-cl-std=CL1.2 -Werror";

    t = clock();
    err = clBuildProgram(my_program, 0, NULL, opt, NULL, NULL);
    t = clock()-t;
    printf(" --- Compiling OpenCL program time: %f\n", (double)t/CLOCKS_PER_SEC);
    
    // Inserido na raça para mostrar os warnings
    printf("\n ---------------------------------------------------------------------------\n");
    printf("\n ---- CL Error: %s\n", clGetErrorString(err));

    // Important
    //
    // log should be dynamically allocated since the build log can grow a lot.
    // The clGetProgramBuildInfo deserves a better implementation...
    char log[10240] = "";
    err = clGetProgramBuildInfo(my_program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(log), log, NULL);
    printf("\n Program build log: \n%s\n\n", log);

    cl_kernel my_kernel;

    // It is possible to create as many kernels as functions defined inside it using
    // clCreateKernelsInProgram(...)
    my_kernel = clCreateKernel(my_program, "mc", NULL);

    cl_float xf[SIZE];
    cl_float yf[SIZE];
    cl_float data[SIZE];

    // Create a buffer with data to my kernel (I'm not using it to write, only to get
    // the values of the kernel)
    cl_mem bufferx, buffery, buffero, bufferb;
    cl_int bound = SIZE;
    
    bufferx = clCreateBuffer(my_context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, SIZE*sizeof(cl_float), xf, NULL);
    buffery = clCreateBuffer(my_context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, SIZE*sizeof(cl_float), yf, NULL);
    buffero = clCreateBuffer(my_context, CL_MEM_WRITE_ONLY, SIZE*sizeof(cl_float), NULL, NULL); 
    bufferb = clCreateBuffer(my_context, CL_MEM_READ_ONLY, sizeof(cl_int), NULL, NULL);

    // A tentative loop to launch N kernels
    // Initialize counters
    unsigned long int cl_total = 0;
    unsigned long int cl_size = 0;
    double cl_pi;

    // Somehow the first two calls to drand48()
    // give 0.00... Check a better way of generate
    // random numbers in [0,1)
    drand48();
    drand48();

    t = clock();

    // This loop call SIZE times the kernel and all function calls needed
    // to perform the calculation
    for(int x=0; x<SIZE; x++)
    {
      for(int c=0; c<SIZE; c++)
	{
	  xf[c]=drand48();
	  yf[c]=drand48();
	  data[c]=0.0;
	}
	
      /* printf("\n --- BEFORE: "); */
      /* for(int c=0; c<SIZE; c++) */
      /* { */
      /*   printf("[%.2f,", xf[c]); */
      /*   printf("%.2f] ", yf[c]); */
      /* } */
      /* printf("\n"); */
	
	
      // Enqueue and execute the kernel
      clEnqueueWriteBuffer(my_queue, bufferx, CL_FALSE, 0, SIZE*sizeof(cl_float), &xf, 0, NULL, NULL);
      clEnqueueWriteBuffer(my_queue, buffery, CL_FALSE, 0, SIZE*sizeof(cl_float), &yf, 0, NULL, NULL);
      clEnqueueWriteBuffer(my_queue, buffero, CL_FALSE, 0, SIZE*sizeof(cl_float), &data, 0, NULL, NULL);
      clEnqueueWriteBuffer(my_queue, bufferb, CL_FALSE, 0, sizeof(cl_int), &bound, 0, NULL, NULL);
	
      clSetKernelArg(my_kernel, 0, sizeof(cl_mem), &bufferx);
      clSetKernelArg(my_kernel, 1, sizeof(cl_mem), &buffery);
      clSetKernelArg(my_kernel, 2, sizeof(cl_mem), &buffero);
      clSetKernelArg(my_kernel, 3, sizeof(cl_int), &bufferb);
	
      size_t global_dim[] = {1, 0, 0}; // Quantas dimensoes?
      size_t work_dim[] = {SIZE, 0, 0};
	

      clEnqueueNDRangeKernel(my_queue, my_kernel, 1, NULL, work_dim, NULL, 0, NULL, NULL);
	
      clEnqueueReadBuffer(my_queue, buffero, CL_FALSE, 0, SIZE*sizeof(cl_float), &data, 0, NULL, NULL);
	
      clFinish(my_queue);
	
      //      printf(" --- After kernel run:\n");
      for(int c=0; c<SIZE; c++)
	{
	  //	printf("%.2f ", data[c]);
	  // Totalize the number of points inside
	  // the circle
	  if(data[c]<point)
	    cl_total++;
	}
      //    printf("\n\n");

      cl_size += SIZE;
    
      t = clock() - t;
    }
    printf(" --- OpenCL launch time elapsed: %f\n", (double)t/CLOCKS_PER_SEC);
    printf(" --- Size of work-group: %ld\n", SIZE);

    

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    printf("\n");
    printf("----------------------------------------------------------------------------------------- \n");
    cl_pi=4*(double)cl_total/cl_size;

    printf(" --- For %d tries OpenCL calculated pi is %.6f and the total of points is %d\n", cl_size, cl_pi, cl_total);
    clReleaseCommandQueue(my_queue);
    clReleaseContext(my_context);

    printf("----------------------------------------------------------------------------------------- \n\n");
    // If find a GPU, give preference to it.
    // Othewise, run in parallel in a CPU
    
    
    // Play with the following elements of openCL:
    // - cl_devide_id
    // - cl_kernel
    // - cl_program
    // - cl_command_queue
    // - cl_context

    return 0;
}
