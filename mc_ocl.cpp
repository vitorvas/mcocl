// Monte Carlo OpenCL teste in C++
//
// Vitor Vasconcelos - vitors@cdtn.br
// 15/12/2017

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_CL_1_2_DEFAULT_BUILD // O hornet tem o intel opencl 1.2

#include <CL/cl.hpp>

#include <iostream>
#include <string>
#include <algorithm>     // Just for the find_if
#include <fstream>       // To read the kernel file
#include <sstream>     // To use stringstream

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
  std::cout << " ------ Using platform: " << defaultPlat.getInfo<CL_PLATFORM_NAME>() << std::endl;
  //  std::cout << " ----- Available extensions: " << defaultPlat.getInfo<CL_PLATFORM_EXTENSIONS>() << std::endl;

  // After the platform, try to get a device
  std::vector<cl::Device> vecDevices;
  defaultPlat.getDevices(CL_DEVICE_TYPE_ALL, &vecDevices);

  // Should check if there is a device... Anyway, just testing.
  cl::Device myDevice = vecDevices.at(0);
  std::string deviceName = myDevice.getInfo<CL_DEVICE_NAME>();

  // That's why Python is getting the cheers: can't easily trim the string
  // Well, more or less easily. But easily means: one command
  deviceName.erase(deviceName.begin(), std::find_if(deviceName.begin(), deviceName.end(), std::bind1st(std::not_equal_to<char>(), ' ')));

  std::cout << " ------ My device is " << deviceName << std::endl;

  // Create a context
  cl::Context context(myDevice);

  cl::Program::Sources sources;

  // Read file mc_ocl.cl and read it in a kernel
  std::ifstream kernelFile("mc.cl");

  // Read the full contents to a string using iterators
  // --- Remember to check how istreambuf and its iterators work
  // A opção abaixo, aparentemente, não é muito eficiente
  //  std::string kernelString((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
  std::stringstream kernelStream;
  kernelStream << kernelFile.rdbuf();
  std::string kernelString = kernelStream.str();

  std::cout << kernelString << std::endl;

  // File source copied to cl::Program::sources
  sources.push_back({kernelString.c_str(), kernelString.length()});

  return 0;
}

