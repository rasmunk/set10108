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

using namespace std;


atomic<int> value;

void increment(shared_ptr <atomic<int>> value) {
    for (unsigned int i = 0; i < 1000000; i++)
        (*value)++;
}


int main() {

    auto value = make_shared < atomic < int >> ();

    auto num_threads = thread::hardware_concurrency();
    vector <thread> threads;

    for (unsigned int i = 0; i < num_threads; i++) {
        threads.push_back(thread(increment, value));
    }

    for (auto &t : threads)
        t.join();

    cout << "Value = " << *value << endl;
}