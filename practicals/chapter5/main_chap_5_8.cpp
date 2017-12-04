#include <mpi.h>
#include <iostream>


using namespace std;

int main() {
    auto result = MPI_Init(nullptr, nullptr);

    if (result != MPI_SUCCESS) {
        cout << "Error - initializing MPI" << endl;
        MPI_Abort(MPI_COMM_WORLD, result);
        return -1;
    }

    int num_procs, my_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    if (my_rank == 0) {
        string str = "Hello World!";
        MPI_Bcast((void *) &str.c_str()[0], str.length() + 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    } else {
        char data[100];
        MPI_Bcast(data, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
        cout << my_rank << ":" << data << endl;
    }

    MPI_Finalize();
    return 0;
}