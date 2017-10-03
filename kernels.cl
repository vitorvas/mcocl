    // After the command queue, how to execute my kernel?
    // This a dummy kernel only to test compilation and execution
    // Only one not used argument
    __kernel void test(__global float *data)
    {
    // circle center point is 0.5
       __private float point = 0.5;

       int id = get_global_id(0);
       data[id] = id;       
    }   

    __kernel void mc(__global float *x, __global float* y, 
                     __global float* data, const unsigned int boundary)
    {
        int id = get_global_id(0);
	float point = 0.5;

	if(id<boundary)
	{
	    data[id] = sqrt(pow(x[id]-point, 2)+pow(y[id]-point, 2));	
 	}
    }	  

