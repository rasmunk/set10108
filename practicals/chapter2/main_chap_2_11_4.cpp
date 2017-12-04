//
//  main_chap_2_10.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 28/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//
// Mandelbrot algorithm

#include <stdio.h>
#include <vector>
#include <math.h>
#include <thread>
#include <future>
#include <iostream>
#include "FreeImage.h"
#include <chrono>
#include <fstream>


using namespace std;
using namespace std::chrono;

const unsigned int max_iterations = 1000;

const unsigned int dim = 8192;


// Mandelbrot dimensions are ([-2.1, 1.0], [-1.3,1.3])
const double xmin = -2.1;
const double xmax = 1.0;
const double ymin = -1.3;
const double ymax = 1.3;


//Conversion from Mandelbrot coordinate to image coordinate

const double integral_x = (xmax - xmin) / static_cast<double>(dim);
const double integral_y = (ymax - ymin) / static_cast<double>(dim);

vector<double> mandelbrot(unsigned int start_y, unsigned int end_y) {
    double x, y, x1, y1, xx = 0.0;
    unsigned int loop_count = 0;
    // Where to store the results
    vector<double> results;
    // loop through each line
    y = ymin + (start_y * integral_y);

    for (unsigned int y_coord = start_y; y_coord < end_y; ++y_coord) {
        x = xmin;
        // Loop through each pixel on the line
        for (unsigned int x_coord = 0; x_coord < dim; ++x_coord) {
            x1 = 0.0, y1 = 0.0;
            loop_count = 0;
            // Calculate Mandelbrot value
            while (loop_count < max_iterations && sqrt((x1 * x1) + (y1 * y1)) < 2.0) {
                ++loop_count;
                xx = (x1 * x1) - (y1 * y1) + x;
                y1 = 2 * x1 * y1 + y;
                x1 = xx;
            }
            // Get value where loop completed
            auto val = static_cast<double>(loop_count) / static_cast<double>(max_iterations);
            // Push this value onto the vector

            results.push_back(val);
            // Increase x based on the integral
            x += integral_x;
        }
        // Increase y based on integral
        y += integral_y;
    }
    // Return vector
    return results;
}


int main() {
    // Get the number of supported threads
    auto iterations = 20;
    auto thread_power = 3;
    ofstream data("mandelbrot_performance.csv", ofstream::out);

    for (int x = 0; x <= thread_power; ++x) {
        auto num_threds = (pow(2, x));
        data << "num_threads_" << num_threds;
        // Determine strip height
        auto strip_height = dim / num_threds;
        // Create futures

        for (int j = 0; j <= iterations; ++j) {
            vector < future < vector < double >> > futures;

            auto start = system_clock::now();
            for (unsigned int i = 0; i < num_threds; i++) {
                // Range is used to determinenumber of values to process
                futures.push_back(async(mandelbrot, i * strip_height, (i + 1) * strip_height));
            }

            // vector to store the results
            vector <vector<double>> results;
            // Get results
            for (auto &f : futures)
                results.push_back(f.get());

            auto stop = system_clock::now();
            auto total = duration_cast<milliseconds>(stop - start).count();
            data << "," << total;
        }
        data << endl;
    }
    data.close();

    FreeImage_Initialise();
    FIBITMAP *bitmap = FreeImage_Allocate(dim, dim, 24);

    if (!bitmap)
        exit(1);

    unsigned int ycount = 0;
    for (size_t i = 0; i < results.size(); ++i) {
        unsigned int j = 0;
        unsigned int xcount = 0;
        while (j < results[i].size()) //reads 1024 * 8192 entries
        {
            if (xcount >= (dim)) {
                xcount = 0;
                ycount++;
            }
            if (ycount >= ((1 + i) * 1024)) {
                break;
            }
            RGBQUAD color;
            color.rgbBlue = (255.0 * results[i][j]);
            color.rgbGreen = (255.0 * results[i][j]);
            color.rgbRed = (255.0 * results[i][j]);
            FreeImage_SetPixelColor(bitmap, ycount, xcount, &color);

            xcount++;
            j++;
        }
    }

    if (FreeImage_Save(FIF_PNG, bitmap, "test.png", 0))
        cout << "Image successfully saved" << endl;

    FreeImage_DeInitialise();

    return 0;
}