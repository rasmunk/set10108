// Producer / Consumer queue


#include <thread>
#include <vector>
#include <iostream>
#include "parallel_q.cpp"
#include <chrono>

parallel_q queue;

void task_push(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        queue.push(i);
    }
}

void task_pop(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        cout << "Popped value is " << *queue.pop() << endl;
    }
}

int main() {
    cout << "Running" << endl;

    thread t2(task_pop, 20);
    thread t1(task_push, 1);
    thread t3(task_push, 19);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
