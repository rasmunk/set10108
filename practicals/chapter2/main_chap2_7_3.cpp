//
//  deadlock2_7_3.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 28/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//
// 2.7.3

#include <stdio.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <condition_variable>

using namespace std;

mutex mut;


void task_1(condition_variable &condition) {
    cout << "Task 1 is waiting" << endl;
    unique_lock <mutex> lk(mut);
    condition.wait(lk);
}


void task_2(condition_variable &condition) {
    cout << "Task 2 is waiting" << endl;
    unique_lock <mutex> lk(mut);
    condition.wait(lk);
}


int main() {

    condition_variable condition;

    thread t1(task_1, ref(condition));
    thread t2(task_2, ref(condition));


    t1.join();
    t2.join();

    cout << "Programs has finished" << endl;

    return 0;
}

