#include <CL/cl.h>
#include <vector>
#include <iostream>

using namespace std;

// Helper method to print OpenCL device info 
void print_opencl_info(vector <cl_device_id> &devices) {
    // Buffers for device name and vendor
    char device_name[1024], vendor[1024];
    // Declare other necessary variables
    cl_uint num_cores;
    cl_long memory;
    cl_uint clock_freq;
    cl_bool available;

    for (auto &d : devices) {
        //Get info for device
        clGetDeviceInfo(d, CL_DEVICE_NAME, 1024, device_name, nullptr);
        clGetDeviceInfo(d, CL_DEVICE_VENDOR, 1024, vendor, nullptr);
        clGetDeviceInfo(d, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &num_cores, nullptr);
        clGetDeviceInfo(d, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_long), &memory, nullptr);
        clGetDeviceInfo(d, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &clock_freq, nullptr);
        clGetDeviceInfo(d, CL_DEVICE_AVAILABLE, sizeof(cl_bool), &available, nullptr);

        // Print info
        cout << "Device: " << device_name << endl;
        cout << "Vendor: " << vendor << endl;
        cout << "Cores: " << num_cores << endl;
        cout << "Memory: " << memory / (1024 * 1024) << "MB" << endl;
        cout << "Clock freq: " << clock_freq << "MHz" << endl;
        cout << "Available: " << available << endl;
        cout << "******************" << endl << endl;
    }
}


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
    print_opencl_info(devices);

    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);

    cin.get();
    return 0;
}