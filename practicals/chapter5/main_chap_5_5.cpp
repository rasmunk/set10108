#include <iostream>
#include <mpi.h>

// Chapter 5.2
using namespace std;

int main() {

    int num_procs, my_rank;

    auto result = MPI_Init(nullptr, nullptr);

    if (result != MPI_SUCCESS) {
        cout << "ERROR - initialising MPI" << endl;
        MPI_Abort(MPI_COMM_WORLD, result);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Check if we are the main process
    if (my_rank != 0) {
        // Generate message
        stringstream buffer;
        buffer << "Greeting from process " << my_rank << " of " << num_procs << "!";
    }

    return 0;
}
