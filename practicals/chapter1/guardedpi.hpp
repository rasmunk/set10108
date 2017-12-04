//
//  guardedpi.hpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 22/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//

#ifndef guardedpi_hpp
#define guardedpi_hpp

#include <stdio.h>
#include <mutex>
#include <vector>

using namespace std;

class guardedpi
{
private:
    vector<double> pies;
    mutex mut;
    int pi;

public:
    // Contructor
    guardedpi() : pi(0) {}

    // Deconstructor
    ~guardedpi() {}

    int get_value() const { return pi; }

    void addPi(double value);

    vector<double> getPies();
};

#endif /* guardedpi_hpp */
