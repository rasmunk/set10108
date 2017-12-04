//
//  main_chap2_9.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 28/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//
// 2.9
// Find maximum value in a vector of random values -> split work across CPU

#include <stdio.h>
#include <future>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <iostream>


using namespace std;
using namespace std::chrono;

unsigned int find_max(const vector<unsigned int> &data, unsigned int start, unsigned int end) {
    unsigned int max = 0;
    for (unsigned int i = start; i < end; i++) {
        if (data.at(i) > max)
            max = data.at(i);
    }
    return max;
}


int main() {

    auto num_threads = thread::hardware_concurrency();

    auto iterations = pow(2, 24);

    vector<unsigned int> values;
    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(static_cast<unsigned int>(millis.count()));

    for (unsigned int i = 0; i < iterations; i++) {
        values.push_back(e());
    }

    vector <future<unsigned int>> futures;

    auto range = static_cast<unsigned int>(iterations / num_threads);
    for (unsigned int i = 0; i < num_threads - 1; i++) {
        futures.push_back(async(find_max, values, i * range, (i + 1) * range));
    }

    auto max = find_max(values, (num_threads - 1) * range, num_threads * range);
    for (auto &f : futures) {
        auto result = f.get();
        if (result > max)
            max = result;
    }

    cout << "Maximum value found: " << max << endl;

    return 0;
}