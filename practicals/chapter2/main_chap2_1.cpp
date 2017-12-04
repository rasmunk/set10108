//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 2.0

#include <thread>
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <fstream>
#include <random>
#include <sstream>

using namespace std;
using namespace std::chrono;

void increment(shared_ptr <atomic<int>> value) {
    for (unsigned int i = 0; i < 1000000; i++) {
        (*value).fetch_add(1);
    }
}


int main() {
    auto num_threads = thread::hardware_concurrency();
    ofstream data("atomic_increment_2_11_2.csv", ofstream::out);

    data << "num_threads_" << num_threads;
    for (unsigned int iters = 0; iters < 100; ++iters) {
        vector <thread> threads;
        auto value = make_shared < atomic < int >> (0);

        auto start = system_clock::now();

        for (unsigned int i = 0; i < num_threads; i++) {
            threads.push_back(thread(increment, value));
        }
        for (auto &t : threads) {
            t.join();
        }
        cout << "Value = " << *value << endl;

        auto end = system_clock::now();
        auto total = (end - start);

        data << ", " << duration_cast<milliseconds>(total).count();
    }
    data << endl << endl;
    //cout << "Value = " << *value << endl;
    data.close();
    return 0;
}