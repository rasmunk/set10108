//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// Chapter 1 challenge

#include <thread>
#include <iostream>
#include <chrono>
#include <fstream>
#include <random>
#include <sstream>

using namespace std;;
using namespace std::chrono;

void work() {
    int n = 0;
    for (int i = 0; i < 1000000; i++)
        ++n;
}

double monte_carlo_pi(unsigned int iterations) {
    auto microsec = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    default_random_engine e(microsec.count());
    uniform_real_distribution<double> distribution(0.0, 1.0);

    int pies[iterations];

    unsigned int in_cicle = 0;
    for (unsigned int i = 0; i < iterations; i++) {
        auto x = distribution(e);
        auto y = distribution(e);

        auto length = sqrt((x * x) + (y * y));
        if (length <= 1.0)
            ++in_cicle;
    }

    auto pi = (4.0 * in_cicle) / static_cast<double>(iterations);
    //cout << "Pi is: " << pi << endl;
    return pi;

}

int main(int argc, const char *argv[]) {
    ofstream data("montecarlo_challenge_v2_time.csv", ofstream::out);
    ofstream data2("montecarlo_challenge_v2_result.csv", ofstream::out);
    for (unsigned int num_threads = 0; num_threads <= 6; ++num_threads) {
        //auto total_threads = static_cast<unsigned int>(pow(2.0,num_threads));
        const int total_threads = thread::hardware_concurrency();

        int iterations = (pow(2, 30));
        //  cout << "Number of threads = " << total_threads;
        data << "num_threads_ " << total_threads << " _iterations_" << iterations << endl;
        data2 << "num_threads_ " << total_threads << " _iterations_" << iterations << endl;
        data << ",Time(ms):";
        data2 << ",Result(pi):";

        for (unsigned int iters = 0; iters < 20; ++iters) {
            double resultsPi[total_threads];
            auto start = system_clock::now();
            vector <thread> threads;
            for (unsigned int n = 0; n < total_threads; ++n) {
                threads.push_back(thread([&resultsPi, n, iterations]() { resultsPi[n] = monte_carlo_pi(iterations); }));
            }
            for (auto &t : threads) {
                t.join();
            }

            double averagePi = 0;
            for (int i = 0; i < total_threads; i++) {
                averagePi = (averagePi + resultsPi[i]);
                if (i == (total_threads - 1))
                    averagePi = (averagePi / total_threads);
            }

            auto end = system_clock::now();
            auto total = end - start;

            cout << "Iterations: " << iterations << endl;
            cout << "Result is: " << averagePi << endl;
            cout << "Time is: " << duration_cast<milliseconds>(total).count() << endl;
            data << "," << duration_cast<milliseconds>(total).count();
            data2 << "," << averagePi;
        }
        data << endl << endl;
        data2 << endl << endl;
    }
    data.close();
    data2.close();
    return 0;
}