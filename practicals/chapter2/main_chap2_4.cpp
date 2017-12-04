//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 2.4 -> semaphore

#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

mutex mut;


void task_1(condition_variable &condition) {
    cout << "Task 1 sleeping for 3 seconds" << endl;
    this_thread::sleep_for(chrono::seconds(3));

    cout << "Task 1 notifying waiting thread" << endl;
    condition.notify_one();

    cout << "Taks 1 waiting for notification" << endl;
    unique_lock <mutex> lk(mut);
    condition.wait(lk);

    cout << "Task 1 notified" << endl;

    cout << "Task 1 sleeping for 3 seconds" << endl;
    this_thread::sleep_for(chrono::seconds(3));


    cout << "Task 1 notifying waiting thread" << endl;
    condition.notify_one();

    cout << "Task 1 waiting 3 seconds for notification" << endl;
    if (condition.wait_for(lk, chrono::seconds(3), []() { return 0; }))
        cout << "Task 1 notified before 3 seconds" << endl;
    else
        cout << "Task 1 got tired waiting" << endl;

    cout << "Task 1 finished" << endl;
}

void task_2(condition_variable &condition) {
    cout << "Task 2 waiting for notification" << endl;
    unique_lock <mutex> lk(mut);
    condition.wait(lk);

    cout << "Task 2 notified" << endl;

    cout << "Task 2 sleeping for 5 seconds" << endl;
    this_thread::sleep_for(chrono::seconds(5));

    cout << "Task 2 notifying waiting thread" << endl;
    condition.notify_one();

    cout << "Task 2 waiting 5 seconds for notification" << endl;
    if (condition.wait_for(lk, chrono::seconds(5), []() { return 0; }))
        cout << "Task 2 notified before 5 seconds" << endl;
    else
        cout << "Task 2 got tired waiting" << endl;

    cout << "Task 2 sleeping for 5 seconds" << endl;
    this_thread::sleep_for(chrono::seconds(5));

    cout << "Task 2 notifying waiting thread" << endl;
    condition.notify_one();

    cout << "Task 2 finished" << endl;
}


int main() {
    condition_variable condition;

    thread t1(task_1, ref(condition));
    thread t2(task_2, ref(condition));

    t1.join();
    t2.join();
    return 0;
}