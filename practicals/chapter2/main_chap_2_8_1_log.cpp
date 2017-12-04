//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 28/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//
// 2.8.1

#include <stdio.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <random>
#include <fstream>

using namespace std;
atomic<int> value;

void increment(shared_ptr <atomic<int>> value, unsigned int iterations) {
    for (unsigned int j = 0; j < iterations; j++) {
        for (unsigned int i = 0; i < 1000000; i++)
            (*value)++;
    }
}

int main() {
    ofstream data("parallel_2_8_2.csv", ofstream::out);
    ofstream data2("parallel_2_8_2_time.csv", ofstream::out);
    const int total_threads = thread::hardware_concurrency();

    int iterations = (pow(2, 10));

    data << "num_threads_ " << total_threads << " _iterations_" << iterations << endl;
    data2 << "num_threads_ " << total_threads << " _iterations_" << iterations << endl;

    data << ",Result(value):";
    data2 << ",Time(ms):";

    for (unsigned int iters = 0; iters < 10; iters++) {
        auto start = chrono::system_clock::now();

        auto value = make_shared < atomic < int >> ();
        auto num_threads = thread::hardware_concurrency();
        vector <thread> threads;

        for (unsigned int i = 0; i < num_threads; i++) {
            threads.push_back(thread(increment, value, iterations));
        }

        for (auto &t : threads)
            t.join();

        // cout << "Value = " << (*value) << endl;

        auto end = chrono::system_clock::now();
        auto total = end - start;

        data << "," << *value;
        data2 << "," << chrono::duration_cast<chrono::milliseconds>(total).count();
    }
    data << endl << endl;
    data2 << endl << endl;
    data.close();
    data2.close();
    return 0;
}