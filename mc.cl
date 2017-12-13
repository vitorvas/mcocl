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
