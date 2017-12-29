// --------------------------------------------------------
// Kernel
//
// 1) In this approach, the random numbers are generated separatedely
//    in the CPU and passed to the kernel.

__kernel void mc(__global const float *x, __global const float *y,
		const float center, uint nsamples, __global float *p)
{
    // Calculate
}
	
// --------------------------------------------------------
// This kernel is for C++ test
// kernel calculates for each element C=A+B

__kernel void simple_add(__global const int* A, __global const int* B, __global int* C)
{
  int i = get_global_id(0);
  C[i]=A[i]+B[i];
}
