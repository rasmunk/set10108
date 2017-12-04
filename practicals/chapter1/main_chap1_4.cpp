//
//  main.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 14/09/15.
//  Copyright (c) 2015 Rasmus Munk. All rights reserved.
//
// 1.4

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <functional>


using namespace std;;
using namespace std::chrono;


int main(int argc, const char *argv[]) {
    cout << "Lambda function" << endl;
    auto add = [](int x, int y) { return x + y; };
    auto x = add(10, 12);
    cout << "10 + 12 = " << x << endl;
    cout << "Functional object" << endl;
    function<int(int, int)> add_function = [](int x, int y) { return x + y; };
    x = add_function(20, 12);
    cout << "20 + 12 = " << x << endl;
    cout << "Adding fixed parameters" << endl;

    int a = 5;
    int b = 10;
    auto add_fixed = [a, b] { return a + b; };

    a = 20;
    b = 30;
    x = add_fixed();
    cout << "5 + 10 = " << x << endl;
    cout << "Adding fixed by reference" << endl;
    auto add_reference = [&a, &b] { return a + b; };
    x = add_reference();
    cout << "20 + 30 = " << x << endl;
    a = 30;
    b = 5;
    x = add_reference();
    cout << "30 + 5 = " << x << endl;
    return 0;
}
