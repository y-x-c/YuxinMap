//
//  main.cpp
//  ShortestPath
//
//  Created by 陈裕昕 on 1/3/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include <iostream>
#include "ShortestPath.h"

int main(int argc, const char * argv[]) {
    std::ios::sync_with_stdio(false);
    
    if(strcmp(argv[3], "walking") == 0)
    {
        ShortPath sp(argv[1], argv[2], 4);
        sp.walking(argv[4], argv[5], argv[6]);
    }
    else if (strcmp(argv[3], "driving") == 0)
    {
        ShortPath sp(argv[1], argv[2], 2);
        sp.driving(argv[4], argv[5], argv[6]);
    }
    
    return 0;
}
