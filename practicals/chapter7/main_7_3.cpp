#include <stdio.h>
#include <CL/cl.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

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


// Load an OpenCL program
cl_program load_program(const string &filename, cl_context &context, cl_device_id &device, cl_int num_devices) {
    cl_int status;

    // Create and compile program
    // Read in kernal file
    ifstream input(filename, ifstream::in);
    stringstream buffer;
    buffer << input.rdbuf();
    // Get the character array of the file contents
    auto file_contents = buffer.str();
    auto char_contents = file_contents.c_str();

    // Create program object
    auto program = clCreateProgramWithSource(context, 1, &char_contents, nullptr, &status);
    // Compile / build program
    status = clBuildProgram(program, num_devices, &device, nullptr, nullptr, nullptr);

    // Check if compiled
    if (status != CL_SUCCESS) {
        // Error building - get log
        size_t length;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &length);
        char *log = new char[length];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, length, log, &length);
        // Print log
        cout << log << endl;
        delete[] log;
    }
    // Return program object
    return program;
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

    // Load program
    auto program = load_program("kernel.cl", context, devices[0], devices.size());

    // Create the kernel
    auto kernel = clCreateKernel(program, "vecadd", &status);

    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);
    clReleaseKernel(kernel);
    clReleaseProgram(program);

    cin.get();
    return 0;
}