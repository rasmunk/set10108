//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 1.8.2

// Exercises

#include <thread>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>
#include <random>
#include <array>


using namespace std;;
using namespace std::chrono;


auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
default_random_engine e(millis.count());

void gen_numbers(int amount) {
    for (int i = 0; i < (amount - 1); i++) {
        int random_int = e();
        numbers.push_back(static_cast<double>(random_int));
    }
}


void populate_vector(vector<int> &numbers, const int amount) {
    for (int i = 0; i < amount; i++) {
        int random_int = rand() % amount;
        //  cout << "Random numb vector: " << random_int << endl;
        numbers.push_back(random_int);
    }
}

void populate_matrix(int matrix[1000][1000], const int amount) {
    for (int i = 0; i < amount; i++) {
        for (int j = 0; j < amount; j++) {
            int random_int = rand() % amount;
            matrix[i][j] = random_int;
            //cout << "Random numb matrix: " << random_int << endl;
        }
    }
}

void matrix_vector_multiplication(int matrix[][1000], vector<int> numbers, const int amount, int iterations) {
    ostringstream output;
    output << "iterations " << iterations << endl;
    output << "Size of amount: " << amount << endl;
    //array<array<int,amount>,amount> matrix;

    output << "Matrix size: " << (sizeof(matrix) / sizeof(matrix[0])) << endl;

    for (int i = 0; i < amount; i++) {
        for (int j = 0; j < amount; j++) {
            int random_int = rand() % amount;
            matrix[i][j] = random_int;
            //cout << "Random numb matrix: " << random_int << endl;
        }
    }
    for (int x = 0; x < iterations; x++) {
        static int resultmatrix[1000][1000];
        for (int i = 0; i < amount; i++) {
            for (int j = 0; j < amount; j++) {
                // output << "Matrix element" << ": " << i << "," << j << " : " << matrix[i][j] << endl;
                int vectref = numbers.at(i);
                resultmatrix[i][j] = (matrix[i][j] * numbers.at(i));
                //output << "Vector element: " << vectref << endl;
                //output << "Result is: " << resultmatrix[i][j] << endl;
            }
        }
        output << "Iteration number: " << x << " Completed" << endl;
    }
    cout << output.str() << endl;
}


int main(int argc, const char *argv[]) {
    const int amount = 1000;
    int iter = 1;
    vector<int> numbers;
    populate_vector(numbers, amount);

    int matrix[amount][amount];
    populate_matrix(matrix, amount);

    matrix_vector_multiplication(1);

    vector <thread> threads;
    for (int i = 0; i < iter; i++) {
        threads.push_back();
    }
    for (auto &t : threads) {
        t.join();
    }

    ofstream data("matrix_vector_multiplication.csv", ofstream::out);
    for (unsigned int num_threads = 0; num_threads <= 6; ++num_threads) {
        auto total_threads = static_cast<unsigned int>(pow(2.0, num_threads));
        //  cout << "Number of threads = " << total_threads;
        data << "num_threads_ " << total_threads;

        for (unsigned int iters = 0; iters < 100; ++iters) {
            auto start = system_clock::now();
            vector <thread> threads;
            for (int n = 0; n < total_threads; ++n) {
                threads.push_back(thread(matrix_vector_multiplication, matrix, numbers, amount,
                                         static_cast<int>(pow(2.0, 10.0 - num_threads))));
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