//
//  main_chap2_8_2.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 28/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//
// 2.8.2

#include <stdio.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>


using namespace std;
using namespace std::chrono;

void task(unsigned int id, shared_ptr<atomic_flag> flag)
{
   
    for (unsigned int i = 0; i < 10; i++)
    {
        while (flag->test_and_set());
        
        cout << "Thread " << id << " running " << i << endl;
        this_thread::sleep_for(seconds(1));
        flag->clear();
    }
    
}




int main()
{
    auto flag = make_shared<atomic_flag>();
    
    auto num_threads = thread::hardware_concurrency();
    
    vector<thread> threads;
    
    for (unsigned int i = 0; i < num_threads; i++) {
        threads.push_back(thread(task, i, flag));
    }
    
    for (auto &t : threads)
        t.join();
    
    return 0;
}
