//
//  guardedpi.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 22/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//

#include "guardedpi.hpp"

void guardedpi::addPi(double value) {
    lock_guard <mutex> lock(mut);
    pies.push_back(value);
}

vector<double> guardedpi::getPies() {
    return pies;
}