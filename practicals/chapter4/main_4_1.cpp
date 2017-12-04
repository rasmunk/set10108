//
//  main_4_1.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 13/10/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//

#include <stdio.h>
#include <iostream>

using namespace std;


int main() {
    // Declare a single 128-bit value aligned to 16 bytes (size of 128-bits)
    __declspec(align(16)) __m128 x;

    x.m128_f32[0] = 10.0f;
    x.m128_f32[1] = 20.0f;
    x.m128_f32[2] = 30.0f;
    x.m128_f32[3] = 40.0f;

    // We can print out individual values
    cout << x.m128_f32[0] << endl;

    // Create an array of SIZE floats aligned to 4 bytes (size of a float)
    float *data = (float *) _aligned_mallock(SIZE * sizeof(float), 4);

    // Access just like an array
    cout << data[0] << endl;

    // Create an array of SIZE 128-bit values aligend to 16 bytes
    __128 *big_data = (__m128 *) _aligned_mallock(SIZE * sizeof(__m128), 16);


    // Access just like an array of __m128
    cout << big_data[0].m128_f32[0] << endl;

    // Free the data - ALWAYS REMEMBER TO FREE YOU MEMORY
    // We are dealing at a C level here
    _aligned_free(data);
    _aligned_free(big_data);

    return 0;
}