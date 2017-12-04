
// 3.1

#include <iostream>
#include <omp.h>

using namespace std;
const int THREADS = 10;

void hello() {
    // Get the thread number
    auto my_rank = omp_get_thread_num();

    // Get the number of threads in operation
    auto thread_count = omp_get_num_threads();
    cout << "Hello from thread " << my_rank << " of " << thread_count << endl;
}


int main() {
#pragma omp parallel num_threads(THREADS)
    hello();

    return 0;
}