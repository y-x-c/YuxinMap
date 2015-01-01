//
//  data_converter.h
//  Filter
//
//  Created by 陈裕昕 on 1/1/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#ifndef __Filter__data_converter__
#define __Filter__data_converter__

#include "utility.h"
#include "pugixml.hpp"
#include "cstring"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

namespace YuxinMap{
    
    class Data_Converter
    {
    private:
        int level;
        link_t &link;
        const ac_t &ac;
        const pugi::xml_node &conf;
        
        double min_lat, max_lat, min_lon, max_lon;
        LL tl_tx, tl_ty, br_tx, br_ty;
        
        tiles_t tiles;
        
        bool check(const pugi::xml_node &elem, const pugi::xml_node &conf);
        
        pugi::xml_node get_layer(pugi::xml_node &elem);
        pugi::xml_node get_method(pugi::xml_node &elem);
        
        void convert_way(pugi::xml_node &way, pugi::xml_node &layer, pugi::xml_node &method);
    public:
        Data_Converter(int level, const pugi::xml_node &bounds, link_t &link, const ac_t &ac, const pugi::xml_node &conf);
        ~Data_Converter();
        
        void convert(const std::string &path, elem_layers_t &elem_layers, elem_plot_methods_t &elem_plot_methods);
    };
    
} // namespace YuxinMap

#endif /* defined(__Filter__data_converter__) */
