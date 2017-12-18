// Monte Carlo OpenCL teste in C++
//
// Vitor Vasconcelos - vitors@cdtn.br
// 15/12/2017

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_CL_1_2_DEFAULT_BUILD // O hornet tem o intel opencl 1.2

#include <CL/cl2.hpp>

#include <iostream>

int main(int argc, char** argv)
{
  // First try using the cpp wrapper
  std::vector<cl::Platform> vecPlatforms;
  cl::Platform::get(&vecPlatforms);
  
  if(vecPlatforms.size()==0)
  {
    std::cout << " --- No platforms found. Check OpenCL installation" << std::endl;
    exit(1);
  }

  cl::Platform defaultPlat = vecPlatforms.at(0);
  std::cout << " --- Using platform: " << defaultPlat.getInfo<CL_PLATFORM_NAME>() << std::endl;

  return 0;
}

