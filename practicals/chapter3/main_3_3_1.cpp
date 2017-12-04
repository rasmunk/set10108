// 3.3.1 & 3.4

#include <vector>
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>
#include <thread>
#include <omp.h>

using namespace std;
using namespace std::chrono;

vector<unsigned int> generate_values(unsigned int size) {
    //random engine
    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(static_cast<unsigned int>(millis.count()));
    vector<unsigned int> data;

    for (unsigned int i = 0; i < size; i++) {
        data.push_back(e());
    }
    return data;
}

// 3.4 function
void parallel_sort(vector<unsigned int> &values) {
    // Get the number of threads
    auto num_threads = thread::hardware_concurrency();
    // Get the number of elements in the vector
    auto n = values.size();
    // Declare the valies used in the loop
    int i, tmp, phase;
    // Declare parallel section

#pragma omp parallel num_threads(num_threads) default(none) shared(values, n) private(i, tmp, phase)
    for (phase = 0; phase < n; ++phase) {
        // Determine which phase of the sort we are in
        if (phase % 2 == 0) {
            // Parallel for loop. Each thread jumps forward 2 so no conflict
#pragma omp for
            for (i = 1; i < n; i += 2) {
                //Check if we should swap values
                if (values[i - 1] > values[i]) {
                    tmp = values[i - 1];
                    values[i - 1] = values[i];
                    values[i] = tmp;
                }
            }
        } else {
            // Parallel for loop. Each thread jumps forward 2 so no conflict
#pragma omp for
            for (i = 1; i < n; i += 2) {
                // CHeck if we should swap values
                if (values[i] > values[i + 1]) {
                    tmp = values[i + 1];
                    values[i + 1] = values[i];
                    values[i] = tmp;
                }
            }
        }

    }
}

// 3.3.1 function
void bubble_sort(vector<unsigned int> &data) {
    for (vector<unsigned int>::size_type i = 0; i != data.size(); i++) {
        for (vector<unsigned int>::size_type j = 0; j != data.size(); j++) {
            if (data[j] > data[j + 1]) {
                auto tmp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = tmp;
            }
        }
    }
}


int main() {
    ofstream results("parallel_bubble_sort.csv", ofstream::out);
    for (unsigned int size = 8; size <= 16; ++size) {
        results << pow(2, size) << ", ";
        for (unsigned int i = 0; i < 100; i++) {
            cout << "Generating " << i << " for " << pow(2, size) << " values" << endl;

            auto data = generate_values(static_cast<unsigned int>(pow(2, size)));
            cout << "Sorting" << endl;
            auto start = system_clock::now();
            parallel_sort(data);
            auto stop = system_clock::now();
            auto total = duration_cast<milliseconds>(stop - start).count();

            // Output Time
            results << total << ",";
        }
        results << endl;
    }
    results.close();
    return 0;
}
