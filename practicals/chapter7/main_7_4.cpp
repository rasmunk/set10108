#include <stdio.h>
#include <CL/cl.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>

using namespace std;


// Send data to the GPU -> copy data to the GPU -> Create it first in main memory with the CPU -> 
// Doesn't output anything


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

    // Load program (kernel file)
    auto program = load_program("kernel.cl", context, devices[0], devices.size());

    // Create the kernel object
    auto kernel = clCreateKernel(program, "vecadd", &status);



    // Number of elements and size of buffer on GPU
    const unsigned int elements = 2048;
    const unsigned int data_size = sizeof(int) * elements;

    // Host data - stored in main memory
    array<int, elements> A;
    array<int, elements> B;
    array<int, elements> C;

    // Initialize input data
    for (unsigned int i = 0; i < elements; ++i)
        A[i] = B[i] = i;


    // Create device buffers - stored on GPU
    cl_mem buffer_A;
    cl_mem buffer_B;
    cl_mem buffer_C;
    // allocate buffer size
    buffer_A = clCreateBuffer(context, CL_MEM_READ_ONLY, data_size, nullptr, &status);
    buffer_B = clCreateBuffer(context, CL_MEM_READ_ONLY, data_size, nullptr, &status);
    buffer_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, data_size, nullptr, &status);

    // Copy host data to device data -> writes our data to the relevant buffers. (Third arguement tells OpenCL whether to wait for the write to complete -> don't really care when writing -> would though when reading)
    status = clEnqueueWriteBuffer(cmd_queue, buffer_A, CL_FALSE, 0, data_size, A.data(), 0, nullptr, nullptr);
    status = clEnqueueWriteBuffer(cmd_queue, buffer_B, CL_FALSE, 0, data_size, B.data(), 0, nullptr, nullptr);


    // Kernel arguments
    // The kernel object, parameter number (index values), size of the parameter, and a pointer to the value
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_A);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_B);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_C);


    clReleaseMemObject(buffer_A);
    clReleaseMemObject(buffer_B);
    clReleaseMemObject(buffer_C);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);
    clReleaseKernel(kernel);
    clReleaseProgram(program);

    return 0;
}