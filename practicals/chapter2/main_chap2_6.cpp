//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 2.6


#include "threadsafe_stack.cpp"
#include <thread>
#include <memory>
#include <mutex>
#include <iostream>


void pusher(shared_ptr <threadsafe_stack<unsigned int>> stack) {
    for (unsigned int i = 0; i < 1000000; ++i) {
        stack->push(i);
        // yield = give priority to another thread = if one is "waiting"
        this_thread::yield();
    }
}


void popper(shared_ptr <threadsafe_stack<unsigned int>> stack) {
    unsigned int count = 0;
    while (count < 1000000) {
        //Try and pop a value
        try {
            auto val = stack->pop();
            ++count;
        }
        catch (exception e) {
            cout << "Item not popped, Exception Happend " << endl;
        }
    }
}


int main() {
    // A thread_safe stack -> shared pointer
    auto stack = make_shared < threadsafe_stack < unsigned int >> ();

    thread t1(popper, stack);
    thread t2(pusher, stack);

    t1.join();
    t2.join();

    cout << "Stack Empty = " << stack->empty() << endl;
    return 0;
}