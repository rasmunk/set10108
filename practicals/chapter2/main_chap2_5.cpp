//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 2.6

#include "guarded.cpp"
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

using namespace std;
mutex mut;

const unsigned int NUM_ITERATIONS = 1000000;
const unsigned int NUM_THREADS = 4;

void task(shared_ptr <guarded> g) {
    // Increment guarded object NUM_ITERATIONS
    for (unsigned int i = 0; i < NUM_ITERATIONS; i++)
        g->increment();
}


int main() {
    // Create guarded object (Shared between the threads) -> shared pointer -> has internal counter to keep track
    auto g = make_shared<guarded>();

    vector <thread> threads;
    for (unsigned int i = 0; i < NUM_THREADS; ++i)
        threads.push_back(thread(task, g));

    for (auto &t : threads)
        t.join();

    cout << "Value = " << g->get_value() << endl;
    return 0;
}