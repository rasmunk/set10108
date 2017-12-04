//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 2.7

#include "guardedpi.cpp"
#include <thread>
#include <iostream>
#include <chrono>
#include <fstream>
#include <random>
#include <sstream>
#include <atomic>

using namespace std;
using namespace std::chrono;
mutex mut;

void monte_carlo_pi(shared_ptr <atomic<double>> resultPi, shared_ptr <atomic_flag> flag, unsigned int iterations) {
    auto microsec = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    default_random_engine e(microsec.count());
    uniform_real_distribution<double> distribution(0.0, 1.0);

    unsigned int in_cicle = 0;
    for (unsigned int i = 0; i < iterations; i++) {
        auto x = distribution(e);
        auto y = distribution(e);

        auto length = sqrt((x * x) + (y * y));
        if (length <= 1.0)
            ++in_cicle;
    }

    auto pi = (4.0 * in_cicle) / static_cast<double>(iterations);
    while (flag->test_and_set());

    *resultPi = (*resultPi + pi);

    flag->clear();
}

int main() {
    ofstream data("montecarlo_challenge_atomic_time.csv", ofstream::out);
    ofstream data2("montecarlo_challenge_atomic_result.csv", ofstream::out);
    for (int x = 12; x <= 14; x++) {

        for (unsigned int num_threads = 0; num_threads < 7; ++num_threads) {
            //auto total_threads = static_cast<unsigned int>(pow(2.0,num_threads));
            const int total_threads = thread::hardware_concurrency();

            int iterations = (pow(2, (2 * x)));
            //  cout << "Number of threads = " << total_threads;
            data << "num_threads_ " << total_threads << " _iterations_" << iterations << endl;
            data2 << "num_threads_ " << total_threads << " _iterations_" << iterations << endl;
            data << ",Time(ms):";
            data2 << ",Result(pi):";

            for (unsigned int iters = 0; iters < 20; ++iters) {
                auto flag = make_shared<atomic_flag>();

                auto resultPi = make_shared < atomic < double >> (0.0);

                auto start = system_clock::now();
                vector <thread> threads;
                for (unsigned int n = 0; n < total_threads; ++n) {
                    threads.push_back(thread(monte_carlo_pi, resultPi, flag, iterations));
                }
                for (auto &t : threads) {
                    t.join();
                }

                auto totalPie = (*resultPi / static_cast<double>(threads.size()));
                // cout << "Total pie is " << totalPie << endl;
                auto end = system_clock::now();
                auto total = end - start;

                /*  cout << "THread size " << threads.size() << endl;
                  cout << "Pi is = " << *resultPi << endl;
                  cout << "totalPie = " << totalPie << endl;
                  */
                // cout << "Iterations: " << iterations << endl;
                // cout << "Time is: " << duration_cast<milliseconds>(total).count() << endl;
                data << "," << duration_cast<milliseconds>(total).count();
                data2 << "," << totalPie;

            }
            data << endl << endl;
            data2 << endl << endl;
        }

    }
    data.close();
    data2.close();
    return 0;
}