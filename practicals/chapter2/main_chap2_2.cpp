//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 2.2

#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>

using namespace std;

mutex mut;

void increment(shared_ptr<int> value) {
    for (unsigned int i = 0; i < 1000000; i++) {
        mut.lock();
        *value = *value + 1;
        mut.unlock();
    }
}

int main() {
    auto value = make_shared<int>(0);
    auto num_threads = thread::hardware_concurrency();
    vector <thread> threads;
    for (unsigned int i; i < num_threads; i++) {
        threads.push_back(thread(increment, value));
    }

    for (auto &t : threads) {
        t.join();
    }
    cout << "Value = " << *value << endl;
}