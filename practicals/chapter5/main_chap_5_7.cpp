#include <iostream>
#include <mpi.h>
#include <random>
#include <math.h>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

// Randomly generate vector values
void generate_data(vector<float> &data) {
    // Create random engine
    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(static_cast<unsigned int>(millis.count()));

    for (unsigned int i = 0; i < data.size(); ++i)
        data[i] = e();

}

// normalises 4D vectors
void normalise_vector(vector<float> &data) {
    // Iterate through each 4-dimensional vector
    for (unsigned int i = 0; i < (data.size() / 4); ++i) {
        // sum the squares of the 4 components
        float sum = 0.0f;
        for (unsigned int j = 0; j < 4; ++j)
            sum += powf(data[(i * 4) + j], 2.0f);

        sum = sqrtf(sum);

        // devide each component by sum
        for (unsigned int j = 0; j < 4; ++j)
            data[(i * 4) + j] /= sum;

    }
}


int main() {
    const unsigned int SIZE = static_cast<unsigned int>(pow(2, 24));
    auto result = MPI_Init(nullptr, nullptr);

    if (result != MPI_SUCCESS) {
        cout << "Error - initializing MPI" << endl;
        MPI_Abort(MPI_COMM_WORLD, result);
        return -1;
    }

    int num_procs, my_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    vector<float> data;
    vector<float> my_data(SIZE / num_procs);

    // if main app
    if (my_rank == 0) {
        // generate data
        data.resize(SIZE);
        generate_data(data);
    }

    cout << "My rank is: " << my_rank << endl;
    // Scatter data, Source, Destionation
    MPI_Scatter(&data[0], SIZE / num_procs, MPI_FLOAT, &my_data[0], SIZE / num_procs, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Normalize local data
    normalise_vector(my_data);

    // Gather the results
    MPI_Gather(&my_data[0], SIZE / num_procs, MPI_FLOAT, &data[0], SIZE / num_procs, MPI_FLOAT, 0, MPI_COMM_WORLD);


    if (my_rank == 0) {
        // Display results - first 10
        for (unsigned int i = 0; i < 10; ++i) {
            cout << "<";
            for (unsigned int j = 0; j < 3; ++j)
                cout << data[(i * 4) + j] << ", ";
            cout << data[(i * 4) + 3] << ">" << endl;
        }
    }
    MPI_Finalize();
    return 0;
}