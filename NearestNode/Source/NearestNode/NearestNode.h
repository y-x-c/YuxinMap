//
//  NearestNode.h
//  NearestNode
//
//  Created by 陈裕昕 on 1/4/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#ifndef __NearestNode__NearestNode__
#define __NearestNode__NearestNode__

#include "pugixml/pugixml.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <map>
#include <string>
#include <vector>

class Coord
{
public:
    double lat, lon;
    Coord(){}
    Coord(double _lat, double _lon):lat(_lat), lon(_lon){}
    
    double getDist(const Coord &c) {
        double dlat = lat - c.lat;
        double dlon = lon - c.lon;
        return dlat * dlat + dlon * dlon;
    }
};

class NearestNode {
private:
    pugi::xml_document osm_doc, conf_doc;
    pugi::xml_node osm, conf;
    std::map<std::string, int> osmid2id;
    std::map<int, std::string> id2osmid;
    std::vector<Coord> pts;
    
    void init();
    
public:
    NearestNode(const std::string &osmPath, const std::string &confPath);
    
    bool check(const pugi::xml_node &elem, const pugi::xml_node &conf);
    int getWayClass(const pugi::xml_node &way, int c0);
    std::string find(double lat, double lon, int c0);
};

#endif /* defined(__NearestNode__NearestNode__) */
