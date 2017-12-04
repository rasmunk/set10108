//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 1.8.1 (quicksort first)

// Exercises

#include <thread>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>
#include <random>

using namespace std;;
using namespace std::chrono;

vector<unsigned int> min_n_max_in(vector<unsigned int> &numbers) {
    unsigned int minvalue = 0;
    unsigned int maxvalue = 0;
    vector<unsigned int> min_n_max_numbers;
    for (int i = 0; i < numbers.size(); i++) {
        if (minvalue > numbers[i])
            minvalue = numbers[i];

        if (maxvalue < numbers[i])
            maxvalue = numbers[i];
    }
    min_n_max_numbers.push_back(minvalue);
    min_n_max_numbers.push_back(maxvalue);

    return min_n_max_numbers;
}

void mySwap(int i, int j, vector<int> &numbers) {
    int temp = numbers[i];
    numbers[i] = numbers[j];
    numbers[j] = temp;
}

void quicksort(vector<int> &numbers, int low, int high) {
    int i = static_cast<int>(low);
    int j = static_cast<int>(high);
    unsigned int pivot = numbers[low + ((high - low) / 2)];

    while (low < j || i < high) {
        while (numbers[i] < pivot) {
            i++;
        }
        while (numbers[j] > pivot) {
            j--;
        }
        if (i <= j) {
            mySwap(i, j, numbers);
            i++;
            j--;
        } else {
            if (low < j)
                quicksort(numbers, low, j);
            if (i < high)
                quicksort(numbers, i, high);
            return;
        }
    }
}

void store_rand_amount_in(vector<int> &numbers, int amount) {
    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(millis.count());
    for (int i = 0; i < amount; i++) {
        int random_int = e();
        numbers.push_back(random_int);
    }
}

void find_min_n_max_in(int iterations) {
    // sort first
    ostringstream output;

    for (unsigned int j = 0; j < iterations; j++) {
        vector<int> numbers;
        store_rand_amount_in(numbers, 10000000);
        quicksort(numbers, 0, numbers.size() - 1);
        output << "Minimum Number is " << numbers[0] << endl;
        output << "Maximum Number is " << numbers[numbers.size() - 1] << endl;
    }
    cout << output.str() << endl;
}

int main() {
    ofstream data("millon_random_quicksort.csv", ofstream::out);
    for (unsigned int num_threads = 1; num_threads <= 6; ++num_threads) {
        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));

        cout << "Number of threads = " << total_threads << endl;
        data << "num_threads_ " << total_threads;
        for (unsigned int iters = 0; iters < 10; ++iters) {
            auto start = system_clock::now();
            vector <thread> threads;
            for (unsigned int n = 0; n < total_threads; ++n) {
                //numbers,static_cast<int>(pow(2.0, 10.0 - num_threads))
                threads.push_back(thread(find_min_n_max_in, pow(2.0, 10.0 - num_threads)));
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
    cout << "Program ended" << endl;
    return 0;
}