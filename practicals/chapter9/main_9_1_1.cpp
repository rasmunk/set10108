#include <vector>
#include <random>
#include <math.h>
#include <chrono>
#include <iostream>
#include <CL/cl.h>
#include <array>
#include <fstream>
#include <sstream>

using namespace std;
using namespace std::chrono;

const int n = 1000;

void generate_num(double numbers[][n], int n) {

    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(millis.count());
    uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int i = 0; i < n; ++i) {
        auto x = distribution(e);
        numbers[0][i] = x;
    }

    for (int i = 0; i < n; ++i) {
        auto y = distribution(e);
        numbers[1][i] = y;
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

    // Initialize OpenCL

    cl_int status;

    vector <cl_platform_id> platforms;
    vector <cl_device_id> devices;
    cl_context context;
    cl_command_queue cmd_queue;
    initialise_opencl(platforms, devices, context, cmd_queue);
    print_opencl_info(devices);

    // Load program
    auto program = load_program("kernel_monte_carlo.cl", context, devices[0], devices.size());

    // Create the kernel
    auto kernel = clCreateKernel(program, "monte_carlo_pi", &status);

    // GPU buffer size
    const unsigned int elements = 1000;
    const unsigned int data_size = sizeof(double) * elements;

    // Populate array
    double numbers[2][n];
    generate_num(numbers, n);

    cl_mem buffer_points;
    buffer_points = clCreateBuffer(context, CL_MEM_READ_ONLY, data_size, nullptr, &status);


    // Allocate memory for points and results on GPU
    for (int i = 0; i < n; ++i)
        cout << numbers[1][i] << endl;


    //Free OpenCL resources
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);
    clReleaseKernel(kernel);
    clReleaseProgram(program);

    return 0;
}