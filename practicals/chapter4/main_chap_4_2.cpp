//
//  main_chap_4_2.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 13/10/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//
// Norminalize a vector -> (set a vector length to 1 )

#include <stdio.h>
#include <math.h>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;


// Size of data to allocate - divide by four to get number of vectors

const unsigned int SIZE = static_cast<unsigned int>(pow(2, 24));
const unsigned int NUM_VECTORS = SIZE / 4;

int main() {
    // Data - aligned to 16 bytes (128-bits)

    auto data = (float *) _aligned_malloc(SIZE * sizeof(float), 16);
    // Initialize data
    for (unsigned int i = 0; i < SIZE; ++i) {
        // Set all values to 1
        data[i];
    }

    auto value = _mm_set1_ps(4.0f);
    auto stream_data = (__m128 *) data;

    auto start = high_resolution_clock::now();
    for (unsigned int i = 0; i < NUM_VECTORS; ++i)
        stream_data[i] = _mm_add_ps(stream_data[i], value);

    auto end = high_resolution_clock::now();

    auto total = duration_cast<microseconds>(end - start).count();
    cout << "SIMD: " << total << "micros" << endl;

    // Free memory
    _aligned_free(data);

    // Declare standard data
    data = new float[SIZE];

    for (unsigned int i = 0; i < SIZE; ++i)
        data[i] = 1.0f;

    // Start timer
    start = high_resolution_clock::now();

    for (unsigned int i = 0; i < SIZE; ++i) {
        data[i] = data[i]
    }
    // End timer
    end = high_resolution_clock::now();

    total = duration_cast<microseconds>(end - start).count();
    cout << "NON SIMD: " << total << "micros" << endl;

    // Free memory
    delete[] data;

    return 0;

}