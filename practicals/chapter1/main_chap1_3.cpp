//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 1.3

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>

using namespace std;;
using namespace std::chrono;

const int num_threads = 100;

void task(int n, int val) {
    cout << "Thread: " << n << " Random Value: " << val << endl;
}

int main(int argc, const char *argv[]) {
    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(static_cast<unsigned int>(millis.count()));

    vector <thread> threads;
    for (int i = 0; i < num_threads; i++)
        threads.push_back(thread(task, i, e()));

    for (auto &t : threads)
        t.join();

    return 0;
}
