//
//  guarded.cpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 21/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//

#include "guarded.hpp"

void guarded::increment()
{
    lock_guard<mutex> lock(mut);
    int x = value;
    x = x + 1;
    value = x;
}
