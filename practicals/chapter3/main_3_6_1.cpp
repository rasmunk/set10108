//3.6.1 & 3.6.2
// Scheduling with parallel -> simple round robin

#include <math.h>
#include <thread>
#include <chrono>
#include <omp.h>
#include <fstream>
#include <iostream>


using namespace std;
using namespace std::chrono;

//Determines the amount of work
double f(unsigned int i) {
    auto start = i * (i + 1) / 2;
    auto end = start + i;

    auto result = 0.0;

    // Loop for number of iterations, calculating sin
    for (auto j = start; j <= end; ++j) {
        result += sin(j);
    }
    return result;
}


int main() {

    auto thread_count = thread::hardware_concurrency();
    // Number of iterations to calculate
    int n = static_cast<int>(pow(2, 14));

    ofstream result("schedule_round_robin.csv", ofstream::out);
    const int schedules = 50;

    for (int j = 1; j <= schedules; j++) {
        result << j << ",";
        // declare sum value
        auto sum = 0.0;

        // start time
        auto start = system_clock::now();
#pragma omp parallel for num_threads(thread_count) reduction(+:sum) schedule(static, j)
        for (auto i = 0; i <= n; ++i)
            sum += f(i);

        //cout << "end: " << end << endl;
        auto end = system_clock::now();
        cout << "Sum is: " << sum << endl;
        auto total = duration_cast<milliseconds>(end - start).count();
        result << total << "," << endl;
    }

    result.close();
    return 0;
}