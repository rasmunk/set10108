//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 1.8

// Exercises

#include <thread>
#include <iostream>
#include <chrono>
#include <fstream>
#include <random>


using namespace std;;
using namespace std::chrono;


vector<unsigned int> numbers;

unsigned int maximum_value() {
    unsigned int maxvalue = 0;
    for (int i = 0; i < numbers.size(); i++) {
        if (numbers[i] > maxvalue) {
            maxvalue = numbers[i];
        }
    }
    return maxvalue;
}


unsigned int minimum_value() {
    unsigned int minvalue = 10000;


    return minvalue;
}

void gen_numbers(unsigned int amount) {

    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(millis.count());

    for (int i = 0; i < (amount - 1); i++) {
        unsigned int random_int = e();
        numbers.push_back(static_cast<double>(random_int));
    }
}


int main(int argc, const char *argv[]) {

    gen_numbers(10000000);
    for (int i = 0; i < numbers.size(); i++) {
        cout << "element number: " << i << " Rand num: " << numbers[i] << endl;
    }

    ofstream data("millon_random.csv", ofstream::out);
    for (unsigned int num_threads = 0; num_threads <= 6; ++num_threads) {
        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));

        //  cout << "Number of threads = " << total_threads;
        data << "num_threads_ " << total_threads;

        for (unsigned int iters = 0; iters < 100; ++iters) {
            auto start = system_clock::now();
            vector <thread> threads;
            for (unsigned int n = 0; n < total_threads; ++n) {
                threads.push_back(thread());
            }
            for (auto &t : threads) {
                t.join();
            }
            auto end = system_clock::now();
            auto total = end - start;
            data << ", " << duration_cast<milliseconds>(total).count();
        }
        data << endl;
    }
    data.close();
    return 0;
}