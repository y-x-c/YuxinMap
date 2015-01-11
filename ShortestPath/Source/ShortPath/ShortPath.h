//
//  ShortPath.h
//  ShortPath
//
//  Created by 陈裕昕 on 1/3/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#ifndef __ShortPath__ShortPath__
#define __ShortPath__ShortPath__

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "pugixml.hpp"

class Coord
{
public:
    double lat, lon;
    Coord(){}
    Coord(double _lat, double _lon):lat(_lat), lon(_lon){}
};

class Edge
{
public:
    int y, c;
    double d; // time
    
    Edge(){}
    Edge(int y, int c, double d):y(y), c(c), d(d){}
};

class HeapElem
{
public:
    int x;
    double d, f;
    
    HeapElem(){}
    HeapElem(int x, double d, double f):x(x), d(d), f(f){}
    
    bool operator<(const HeapElem &elem2) const
    {
        return f > elem2.f;
    }
};

class ShortPath
{
private:
    pugi::xml_document osm_doc, conf_doc;
    
    const double INF = 1e60;
    enum Mask
    {
        bi = 1, // bidirection
        dr = 2, // driving
        wa = 4, // walking
        tr = 8  // transit
    };
    int C;
    std::map<std::string, int> osmid2id;
    std::vector<Coord> pts;
    std::vector<std::vector<Edge > > edges;
    pugi::xml_node osm, conf;
    
    bool check(const pugi::xml_node &elem, const pugi::xml_node &conf);
    
    int getWayClass(const pugi::xml_node &way, double &speed);
    
    double getDist(int x, int y);
    void addedges(int x, int y, int c, double speed);
    void init();

    void findShortPath(int S, int T, std::vector<int> &path);
    
    void output(const std::vector<int> &ret, const std::string &outputPath);
public:
    ShortPath(const std::string &osmPath, const std::string &confPath, int c);
    ~ShortPath(){}
    
    void walking(std::string _S, std::string _T, const std::string &outputPath);
    void driving(std::string _S, std::string _T, const std::string &outputPath);
};

#endif /* defined(__ShortPath__ShortPath__) */
