//
//  level_filter.cpp
//  Filter
//
//  Created by 陈裕昕 on 1/1/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include "level_filter.h"
#include "cstring"

namespace YuxinMap{
    bool Level_Filter::check(const pugi::xml_node &elem, const pugi::xml_node &conf)
    {
        // name should be check in advance
        
        // attributes
        for(auto &attr_conf : conf.attributes())
        {
            if(strcmp(elem.attribute(attr_conf.name()).value(), attr_conf.value()) != 0)
            {
                return false;
            }
        }
        
        // tags
        for(auto &child_conf : conf.children())
        {
            bool flag = false;
            for(auto &child_elem : elem.children(child_conf.name()))
            {
                if(check(child_elem, child_conf))
                {
                    flag = true;
                    break;
                }
            }
            if(!flag) return false;
        }
        
        return true;
    }

    void Level_Filter::filter(pugi::xml_node conf, ac_t &ac)
    {
        for(auto it_elem = osm.begin(); it_elem != osm.end(); ++it_elem)
        {
            std::string elem_name = getName(*it_elem);
            
            if(ac.count(elem_name)) continue;
            
            for(auto &conf : conf.children(it_elem->name()))
            {
                if(check(*it_elem, conf))
                {
                    ac.insert(elem_name);
                    break;
                }
            }
        }
    }

}// namespace YuxinMap