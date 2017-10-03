    // After the command queue, how to execute my kernel?
    // This a dummy kernel only to test compilation and execution
    // Only one not used argument
    __kernel void test(__global float *data)
    {
    	int id = get_global_id(0);
	    data[id] = id;
	    float x = 1.1;
	    float y = 2.2;
	    data[id] = id*x*y;

    }   

    __kernel void mc(__global float *x, __global float* y, 
                     __global float* data, const unsigned int boundary)
    {
        int id = get_global_id(0);
	if(id<boundary)
	{
            __private float point = 0.5;
	}
    }	  

