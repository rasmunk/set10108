#include <iostream>
#include <mpi.h>

// Chapter 5.2
using namespace std;

int main() {
    auto result = MPI_Init(nullptr, nullptr);

    // Check initialization
    if (result != MPI_SUCCESS) {
        cout << "ERROR - initialising MPI!" << endl;
        MPI_Abort(MPI_COMM_WORLD, result);
        return -1;
    }

    // Get MPI Information
    int num_procs, rank, length;
    char host_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(host_name, &length);


    // Display information
    cout << "Number of processors = " << num_procs << endl;
    cout << "My rank = " << rank << endl;
    cout << "Running on = " << host_name << endl;

    // Shutdown MPI
    MPI_Finalize();

    return 0;
}