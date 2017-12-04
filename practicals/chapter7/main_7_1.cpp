
#include <stdio.h>
#include <CL/cl.h>
#include <vector>

using namespace std;

void initialise_opencl(vector <cl_platform_id> &platforms, vector <cl_device_id> &devices, cl_context &context,
                       cl_command_queue &cmd_queue) {
    // Status of OpenCL calls
    cl_int status;

    // Get the number of platforms
    cl_uint num_platforms;
    status = clGetPlatformIDs(0, nullptr, &num_platforms);
    // Resize vector to store platforms
    platforms.resize(num_platforms);
    // Fill in platform vector
    status = clGetPlatformIDs(num_platforms, &platforms[0], nullptr);

    // Assume platform 0 is the one we wnat to use
    // Get devices for platform 0
    cl_uint num_devices;
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, nullptr, &num_devices);

    // Resize vector to store devices
    devices.resize(num_devices);
    // Fill in devices vector
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_devices, &devices[0], nullptr);

    // Create a context
    context = clCreateContext(nullptr, num_devices, &devices[0], nullptr, nullptr, &status);

    cmd_queue = clCreateCommandQueue(context, devices[0], 0, &status);
}


int main() {

    // Status of OpenCL calls
    cl_int status;

    // Initialise OpenCL
    vector <cl_platform_id> platforms;
    vector <cl_device_id> devices;
    cl_context context;
    cl_command_queue cmd_queue;
    initialise_opencl(platforms, devices, context, cmd_queue);

    // Free OpenCL resources
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);

    return 0;
}