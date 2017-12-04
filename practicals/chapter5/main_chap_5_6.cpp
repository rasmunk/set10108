#include <mpi.h>
#include <iostream>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;


double monte_carlo_pi(unsigned int iterations) {
    auto microsec = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    default_random_engine e(microsec.count());
    uniform_real_distribution<double> distribution(0.0, 1.0);

    unsigned int in_cicle = 0;
    for (unsigned int i = 0; i < iterations; i++) {
        auto x = distribution(e);
        auto y = distribution(e);

        auto length = sqrt((x * x) + (y * y));
        if (length <= 1.0)
            ++in_cicle;
    }

    auto pi = (4.0 * in_cicle) / static_cast<double>(iterations);
    return pi;
}


int main() {
    
    auto result = MPI_Init(nullptr, nullptr);

    if (result != MPI_SUCCESS) {
        cout << "Error - initializing MPI" << endl;
        MPI_Abort(MPI_COMM_WORLD, result);
        return -1;
    }

    int num_proces, my_rank, length;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Comm_size(MPI_COMM_WORLD, &num_proces);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Get_processor_name(hostname, &length);

    double local_sum, global_sum;
    local_sum = monte_carlo_pi(static_cast<unsigned int>(pow(2, 24)));
    // Print out local sum
    cout.precision(numeric_limits<double>::digits10);
    cout << my_rank << ":" << local_sum << endl;
    // reduce
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


    if (my_rank == 0) {
        global_sum /= 4.0;
        cout << "Pi=" << global_sum << endl;
    }

    MPI_Finalize();

    return 0;
}
