//
//  utility.h
//  Filter
//
//  Created by 陈裕昕 on 1/1/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#ifndef __Filter__utility__
#define __Filter__utility__

#include "pugixml.hpp"
#include <set>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <iostream>

namespace YuxinMap {
    const int tile_l = 256;
    const double sampling_factor = 2;
    const double retina_factor = 2;
    const double PI = acos(-1.0);
    typedef long long LL;
    typedef std::set<std::string> ac_t;
    typedef std::map<std::string, pugi::xml_node> link_t;
    typedef std::map<std::string, pugi::xml_node> elem_layers_t;
    typedef std::map<std::string, pugi::xml_node> elem_plot_methods_t;
    
//    typedef std::vector<std::string> ret_t;
    
    class Coord
    {
    public:
        double lat, lon;
        int level;
        
        double px, py;
        LL tx, ty;
        
        Coord(double lat, double lon, int level)
        {
            this->lat = lat;
            this->lon = lon;
            this->level = level;
            
            double sin_lat = sin(lat * PI / 180.0);
            double n = pow(2, level);
            
            px = (lon + 180.0) / (360.0) * tile_l * n;
            py = (0.5 - log((1 + sin_lat) / (1 - sin_lat)) / (4 * PI)) * tile_l * n;
            
            tx = LL(px / tile_l);
            ty = LL(py / tile_l);
        }
        
        void get_x_y(LL &x, LL &y, LL tx = -1, LL ty = -1)
        {
            if(tx == -1 || ty == -1) tx = this->tx, ty = this->ty;
            
            x = LL((px - tx * tile_l) * sampling_factor * retina_factor);
            y = LL((py - ty * tile_l) * sampling_factor * retina_factor);
        }
    };
    
    typedef std::vector<std::string> tile_layer_elems_t; // elements in the tile
    typedef std::map<int, tile_layer_elems_t> tile_layers_t;
    typedef std::map<LL, std::map<LL, tile_layers_t> > tiles_t; // all tiles within the osm file

    static std::string getName(const pugi::xml_node &elem)
    {
        std::string name;
        const std::string elem_name = elem.name();
        const std::string _id = elem.attribute("id").value();
        if(elem_name == "nd")
            name = "n" + std::string(elem.attribute("ref").value());
        else if(elem_name == "node")
            name = "n" + _id;
        else if (elem_name == "way")
            name = "w" + _id;
        else if (elem_name == "relation")
            name = "r" + _id;
        else if (elem_name == "member")
        {
            const std::string type = elem.attribute("type").value();
            const std::string ref = elem.attribute("ref").value();
            if(type == "way")
                name = "w" + ref;
            else if(type == "node")
                name = "n" + ref;
            else if(type == "relation")
                name = "r" + ref;
        }
        
        return name;
    }
    
    static int h2d(char ch) {
        if(isdigit(ch)) return ch - '0';
        if(ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
        return ch - 'a' + 10;
    }
    
} // namespace YuxinMap


#endif /* defined(__Filter__utility__) */
