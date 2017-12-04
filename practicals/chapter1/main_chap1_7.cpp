//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 1.7

#include <thread>
#include <iostream>
#include <chrono>
#include <fstream>
#include <random>
#include <sstream>

using namespace std;;
using namespace std::chrono;

void work()
{
    int n = 0;
    for (int i=0; i< 1000000; i++)
        ++n;
}

int const iterations = 100;

void monte_carlo_pi(unsigned int iterations)
{
    ostringstream output;
    
    output << "Iterations are: " << iterations << endl;
    auto millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    default_random_engine e(millis.count());
    uniform_real_distribution<double> distribution(0.0, 1.0);
    
    unsigned int in_cicle = 0;
    
    for (unsigned int i=0; i < iterations; i++) {
        auto x = distribution(e);
        auto y = distribution(e);
        
        auto length = sqrt((x*x) + (y*y));
        if (length <= 1.0)
            ++in_cicle;
    }
    
    auto pi = (4.0 * in_cicle) / static_cast<double>(iterations);
    //cout << "Pi is "  << pi << endl;
    cout << output.str() << endl;
    
}

int main(int argc, const char * argv[]) {
    ofstream data("montecarlo_1_8_3.csv", ofstream::out);
    for(unsigned int num_threads = 0; num_threads <= 6; ++num_threads)
    {
        auto total_threads = static_cast<unsigned int>(pow(2.0,num_threads));
                                                       
      //  cout << "Number of threads = " << total_threads;
        data << "num_threads_ " << total_threads;
        
        for (unsigned int iters = 0; iters < 100; ++iters)
        {
            auto start = system_clock::now();
            vector<thread> threads;
            for(unsigned int n = 0; n < total_threads; ++n)
            {
                threads.push_back(thread([num_threads]{ monte_carlo_pi(static_cast<unsigned int>(pow(2.0, 24.0 - num_threads))) ;}));
            }
            for (auto &t : threads)
            {
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