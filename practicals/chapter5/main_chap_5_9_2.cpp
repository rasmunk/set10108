#include <iostream>
#include <chrono>
#include <mpi.h>
#include <vector>
#include <math.h>
#include <FreeImage.h>

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
    cout << "mandelbrot results size: " << results.size() << endl;
    return results;
}


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
    auto strip_height = dim / num_procs;

    //MPI_Scatter(&mandelResults[0], strip_height, MPI_DOUBLE, &myMandel[0], strip_height, MPI_DOUBLE,0,MPI_COMM_WORLD);

    unsigned int mystart = strip_height * my_rank;
    unsigned int myend = strip_height * (my_rank + 1);
    unsigned int myMandelSize = (strip_height * dim);

    cout << "myrank: " << my_rank << " mystart: " << mystart << " myend: " << myend << endl;
    vector<double> mandelResults;

    if (my_rank == 0) {
        mandelResults.resize(dim * dim);
    }

    vector<double> &myMandel = mandelbrot(mystart, myend);
    cout << "Ready to gather" << endl;
    MPI_Gather(&myMandel[0], myMandelSize, MPI_DOUBLE, &mandelResults[0], myMandelSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    cout << "Last element: " << mandelResults.back() << " size: " << mandelResults.size() << endl;
    // Visualize
    FreeImage_Initialise();
    FIBITMAP *bitmap = FreeImage_Allocate(dim, dim, 24);

    if (!bitmap)
        exit(1);

    unsigned int ycount = 0;
    unsigned int xcount = 0;

    for (size_t i = 0; i < mandelResults.size(); ++i) {
        if (ycount == dim) {
            xcount++;
            ycount = 0;
        }

        RGBQUAD color;
        color.rgbBlue = (255.0 * mandelResults[i]);
        color.rgbGreen = (255.0 * mandelResults[i]);
        color.rgbRed = (255.0 * mandelResults[i]);
        FreeImage_SetPixelColor(bitmap, ycount, xcount, &color);

        ycount++;
    }

    if (FreeImage_Save(FIF_PNG, bitmap, "mpi_mandelbrot_test.png", 0))
        cout << "Image successfully saved" << endl;

    FreeImage_DeInitialise();

    return 0;
}