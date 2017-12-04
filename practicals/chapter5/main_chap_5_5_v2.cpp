#include <iostream>
#include <mpi.h>
#include <sstream>

// Chapter 5.2
using namespace std;

int main() {

    const unsigned int MAX_STRING = 100;
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
        // Not main process - send message
        // Generate message
        stringstream buffer;
        buffer << "Greeting from process " << my_rank << " of " << num_procs << "!";

        // Get character array from the string
        auto data = buffer.str().c_str();

        // Send to the main node
        MPI_Send((void *) data, buffer.str().length() + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        // Main process - print message
        cout << "Greetings from process " << my_rank << " of " << num_procs << "!" << endl;

        char message[MAX_STRING];
        for (int i = 1; i < num_procs; ++i) {
            // REceive message into buffer
            MPI_Recv(message, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Display Message
            cout << message << endl;
        }
    }
    MPI_Finalize();

    return 0;
}
