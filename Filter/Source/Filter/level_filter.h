//
//  level_filter.h
//  Filter
//
//  Created by 陈裕昕 on 1/1/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#ifndef __Filter__level_filter__
#define __Filter__level_filter__

#include <iostream>
#include <vector>
#include <map>
#include "pugixml.hpp"
#include "utility.h"

namespace YuxinMap{

    class Level_Filter
    {
    private:
        pugi::xml_node osm;
        
        bool check(const pugi::xml_node &elem, const pugi::xml_node &conf);
        
    public:
        Level_Filter(pugi::xml_node osm)
        {
            this->osm = osm;
        }
        
        ~Level_Filter(){}
        
        void filter(pugi::xml_node conf, ac_t &ac);
    };

    
}// namespace YuxinMap

#endif /* defined(__Filter__level_filter__) */