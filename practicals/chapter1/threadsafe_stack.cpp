//
//  threadsafe_stack.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 21/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//

#pragma once

#include <exception>
#include <stack>
#include <memory>
#include <mutex>

using namespace std;

template<typename T>
class threadsafe_stack
{
private:
    stack <T> data;
    // THe mutex that contols access
    mutable mutex mut;
public:
    // Contructor
    threadsafe_stack() {}

    // Copy Contructor
    threadsafe_stack(const threadsafe_stack &other) {
        // Copy the data from the other stack -> lock other stack
        lock_guard <mutex> lock(other.mut);
        data = other.data;
    }

    void push(T value) {
        // Lock access to the object
        lock_guard <mutex> lock(mut);
        // push to internal stack
        data.push(value);
    }

    T pop() {
        // Lock access ot the object -> until the current scope is done
        lock_guard <mutex> lock(mut);
        if (data.empty()) {
            throw exception();
        }

        auto res = data.top();
        data.pop();

        return res;
    }

    bool empty() const {
        lock_guard <mutex> lock(mut);
        return data.empty();
    }

};


