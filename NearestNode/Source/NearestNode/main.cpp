//
//  main.cpp
//  NearestNode
//
//  Created by 陈裕昕 on 1/4/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include "NearestNode.h"

int main(int argc, const char * argv[]) {
    std::ofstream out(argv[3], std::ofstream::out);
    int c0;
    std::string c0_(argv[6]);
    
    if(!c0_.length() || c0_ == "all")
        c0 = -1;
    else if(c0_ == "driving")
        c0 = 2;
    else if(c0_ == "walking")
        c0 = 4;
    else if(c0_ == "transit")
        c0 = 8;
    
    NearestNode NN(argv[1], argv[2]);
    
    out << NN.find(atof(argv[4]), atof(argv[5]), c0);
    
    return 0;
}
