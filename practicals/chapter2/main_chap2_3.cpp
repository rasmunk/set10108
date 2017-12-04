//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 2.3

#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

mutex mut;

void increment(shared_ptr<int> value) {
    for (unsigned int i = 0; i < 1000000; i++) {
        // a lock/mutex is like a gate
        lock_guard <mutex> lock(mut);
        *value = *value + 1;
        // lock guard is automatically destroyed at the end of the loop scope
        // Will realse the lock
    }
}

int main() {
    auto num_threads = thread::hardware_concurrency();
    ofstream data("mutex_increment_2_3.csv", ofstream::out);
    data << "num_threads_" << num_threads;

    for (unsigned int iters = 0; iters < 10; iters++) {
        vector <thread> threads;
        auto value = make_shared<int>(0);

        auto start = system_clock::now();
        for (unsigned int i = 0; i < num_threads; i++) {
            threads.push_back(thread(increment, value));
        }

        for (auto &t : threads) {
            t.join();
        }

        auto stop = system_clock::now();
        auto total = stop - start;
        data << "," << duration_cast<milliseconds>(total).count();
    }

    data << endl << endl;
    data.close();
    return 0;
}