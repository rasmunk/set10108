//
//  main_chap_4_3.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 13/10/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//

#include <stdio.h>
#include <chrono>
#include <random>
#include <iostream>

using namespace std;
using namespace std::chrono;


// Randomly generate vector values

void generate_data(float *data, unsigned int num_values) {
    // Create random engine
    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    default_random_engine e(static_cast<unsigned int>(millis.count()));
    // Fill data

    for (unsigned int i = 0; i < num_values; ++i)
        data[i] = e();
}

// Normalises the vector
void normalise_vectors(__m128 *data, __m128 *result, unsigned int num_vectors) {
    // Normalise the vectors
    for (unsigned int i = 0; i < num_vectors; ++i) {
        // Square each component - simply multiply the vectors by themselves

        result[i] = _mm_mul_ps(data[i], data[i]);
        // Calculate sum of the components. Store in all
        result[i].m128_f32[0] = result[i].m128_f32[1] = result[i].m128_f32[2] = result[i].m128_f32[3] =
                result[i].m128_f32[0] + result[i].m128_f32[1] + result[i].m128_f32[2] + result[i].m128_f32[3];
        // Calculate recipriol square root of the values
        // That is 1.0f / sqrt(value) - or the length of the vector
        result[i] = _mm_rsqrt_ps(result[i]);

        // Multiply the result by the original data
        // As we have the recipricol it is the same as dividing each component by the length
        result[i] = _mm_mul_ps(data[i], result[i]);
    }
}

// Check the first 100 results
void check_results(__m128 *data, __m128 *result) {
    // Convert to floats
    auto float_data = (float *) data;
    auto float_res = (float *) result;

    for (unsigned int i = 0; i < 100; ++i) {

        // Calucate the length of the vector
        float l = 0.0f;
        // sqaure each component and add to 1
        for (unsigned int j = 0; j < 4; j++) {
            l += powf(float_data[(i * 4) + j], 2.0f);
        }
        // Square the length
        l = sqrtf(l);

        // Now check that the individual results
        for (unsigned int j = 0; j < 4; ++j) {
            cout << float_data[(i * 4) + j] / l << " : " << float_res[(i * 4) + j] << endl;
        }
    }
}
