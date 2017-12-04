//
//  guarded.hpp
//  Concurrent_and_parallal_workbook
//
//  Created by Rasmus Munk on 21/09/15.
//  Copyright © 2015 Rasmus Munk. All rights reserved.
//

#ifndef guarded_hpp
#define guarded_hpp

#include <stdio.h>
#include <mutex>

using namespace std;

class guarded
{
private:
    mutex mut;
    int value;
    
public:
    // Contructor
    guarded() : value(0) { }
    // Deconstructor
    ~guarded() { }
    
    int get_value() const { return value; }
    void increment();
};


#endif /* guarded_hpp */
