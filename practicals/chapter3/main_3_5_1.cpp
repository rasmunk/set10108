#include <omp.h>
#include <iostream>
#include <thread>

#define _USE_MATH_DEFINES // for C++

#include <ATLComTime.h>

using namespace std;


// 3.5.1 function
// Calculates the area under a curve -> more points -> more accuarate
// https://www.khanacademy.org/math/integral-calculus/indefinite-definite-integrals/riemann-sums/v/trapezoidal-approximation-of-area-under-curve
void trap(function<double(double)> f, double start, double end, unsigned int iterations, shared_ptr<double> p) {
    // Get thread number
    auto my_rank = omp_get_thread_num();
    // Get the number of threads
    auto thread_count = omp_get_num_threads();
    // Calculate iteration slice size
    auto slice_size = (end - start) / iterations;

    // Calculate number of iterations per thread -> or trapezioeds
    auto iterations_thread = iterations / thread_count;

    // Calculate this thread's start point
    auto local_start = start + ((my_rank * iterations_thread) * slice_size);
    // Calculate this threads end point
    auto local_end = local_start + iterations_thread * slice_size;

    // Calculate initial result
    auto my_result = (f(local_start) + f(local_end)) / 2.0;

    // Declare x before the loop - stops it being allocated and destroyed each iteration
    double x;

    for (unsigned int i = 0; i < iterations_thread; i++) {
        // Calculate next slice to calculate
        x = local_start + i * slice_size;
        // Add to current result
        my_result += f(x);
    }
    // Multiply the result by the slice size
    my_result *= slice_size;

#pragma omp critical
    *p += my_result;
}


int main() {
    // Declare shared result
    auto result = make_shared<double>(0.0);
    // Define start and end values

    auto start = 0.0;
    auto end = M_PI;

    unsigned int trapeziods = static_cast<unsigned int>(pow(2, 31));

    auto thread_count = 8;

    // Create function to calculate integral of . Use cos or sin
    auto f = [](double x) { return cos(x); };


    // Run trap in parallel
#pragma omp parallel num_threads(thread_count)
    trap(f, start, end, trapeziods, result);

    cout << "Using " << trapeziods << " trapezoids. ";
    cout << "Estimated integral of function " << start << " to " << end << " = " << *result << endl;

    return 0;
}