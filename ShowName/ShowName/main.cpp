//
//  main.cpp
//  Filter
//
//  Created by 陈裕昕 on 12/31/14.
//  Copyright (c) 2014 Fudan. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "pugixml.hpp"
#include "utility.h"

pugi::xml_document osm_doc;
pugi::xml_document conf_doc;
pugi::xml_node bounds, osm, conf;

YuxinMap::ac_t ac;
YuxinMap::elem_layers_t elem_layers;
YuxinMap::elem_plot_methods_t elem_plot_methods;
YuxinMap::link_t link;

using namespace std;

namespace YuxinMap {

    const int iconPix = 16;
    const int pixPerChar = 16;
    const int extraPix = 40;
    
    struct Rect {
        LL tlx, tly;
        LL brx, bry;
        
        Rect(LL tlx, LL tly, LL brx, LL bry):tlx(tlx), tly(tly), brx(brx), bry(bry){}
        
        bool intersect(const Rect &b) const {
            LL minbrx = min(brx, b.brx);
            LL minbry = min(bry, b.bry);
            LL maxtlx = max(tlx, b.tlx);
            LL maxtly = max(tly, b.tly);
            
            if(minbrx >= maxtlx && minbry >= maxtly) return true;
            
//            cerr << tlx << " " << tly << " " << brx << " " << bry << endl;
//            cerr << b.tlx << " " << b.tly << " " << b.brx << " " << b.bry << endl;
            
            return false;
        }
    };
    
    //vector<vector<vector<Rect> > > rects;
    vector<Rect> rects;
    vector<vector<vector<string> > > names;
    
    void checkAndPush(const string &name, LL x, LL y, int tx, int ty, int tl_tx, int tl_ty, double lat, double lon) {
        LL tlx, tly, brx, bry;
        tlx = x; tly = y;
        brx = name.size() * pixPerChar + tlx + iconPix + extraPix;
        bry = tly + iconPix + extraPix;
        Rect cur(tlx, tly, brx, bry);
        
        bool noIntersection = true;
        //for(auto &rect : rects[tx - tl_tx][ty - tl_ty]) {
        for(auto &rect : rects) {
            if(rect.intersect(cur)) {
                noIntersection = false;
                break;
            }
        }
        
        if(noIntersection) {
            //rects[tx - tl_tx][ty - tl_ty].push_back(cur);
            rects.push_back(cur);
            
            stringstream ss;
            ss << "node " << lat << " " << lon << " " << name << " \n";
            names[tx - tl_tx][ty - tl_ty].push_back(ss.str());
        }
    }
    
    void gen(int level, pugi::xml_node &bounds, string &path) {
        double min_lat, max_lat, min_lon, max_lon;
        min_lat = bounds.attribute("minlat").as_double();
        max_lat = bounds.attribute("maxlat").as_double();
        min_lon = bounds.attribute("minlon").as_double();
        max_lon = bounds.attribute("maxlon").as_double();
        
        
        LL tl_tx, tl_ty, br_tx, br_ty;
        Coord tl(max_lat, min_lon, level), br(min_lat, max_lon, level);
        tl_tx = tl.tx; tl_ty = tl.ty;
        br_tx = br.tx; br_ty = br.ty;
        
        LL _x, _y;
        tl.get_x_y(_x, _y);
        if(_x != 0) tl_tx++;
        if(_y != 0) tl_ty++;
        
        br.get_x_y(_x, _y);
        if(_x != tile_l - 1) br_tx--;
        if(_y != tile_l - 1) br_ty--;
        
        if(tl_tx > br_tx || tl_ty > br_ty) return;
        
        rects.clear();
        names.clear();
//        rects.resize(br_tx - tl_tx + 1);
        names.resize(br_tx - tl_tx + 1);
        for(LL tx = tl_tx; tx <= br_tx; tx++) {
            //rects[tx - tl_tx].resize(br_ty - tl_ty + 1);
            names[tx - tl_tx].resize(br_ty - tl_ty + 1);
        }
        
        for(auto &node : osm.children("node")) {
            pugi::xml_node tag = node.find_child_by_attribute("tag", "k", "name");
            
            if(!tag) continue;
            if(node.find_child_by_attribute("tag", "v", "subway_entrance")) continue;
            
            string name = tag.attribute("v").as_string();
            
            double lat, lon;
            lat = node.attribute("lat").as_double();
            lon = node.attribute("lon").as_double();
            Coord pt(lat, lon, level);
            
            if(pt.tx < tl_tx || pt.ty < tl_ty || pt.tx > br_tx || pt.ty > br_ty) continue;
            LL x, y;
            pt.get_x_y(x, y, 0, 0);
            
            checkAndPush(name, x, y, pt.tx, pt.ty, tl_tx, tl_ty, lat, lon);
        }
        
        for(LL tx = tl_tx; tx <= br_tx; tx++) {
            for(LL ty = tl_ty; ty <= br_ty; ty++) {
                stringstream ss;
                ss << path << tx << "_" << ty << "_" << level << ".namedat";
                ofstream out(ss.str().c_str(), std::ofstream::out);
                
                out << names[tx - tl_tx][ty - tl_ty].size() << endl;

                for(auto &name : names[tx - tl_tx][ty - tl_ty]) {
                    out << name;
                }
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    std::ios::sync_with_stdio(false);
    
    pugi::xml_parse_result result_osm = osm_doc.load_file(argv[1]);
    std::cout << "OSM load result: " << result_osm.description() << std::endl;
    if(!result_osm) return -1;
    
    pugi::xml_parse_result result_conf = conf_doc.load_file(argv[2]);
    std::cout << "Conf load result: " << result_conf.description() << std::endl;
    if(!result_conf) return -2;
    
    osm = osm_doc.child("osm");
    conf = conf_doc.child("YuxinMap");
    bounds = osm.child("bounds");
    
    for(auto &node : osm.children())
    {
        link[YuxinMap::getName(node)] = node;
    }

    std::string path(argv[3]);
    int min_level = atoi(argv[4]), max_level = atoi(argv[5]);
    
    for(int level = min_level; level <= max_level; level++)
    {
//        std::cerr << level << std::endl;
        YuxinMap::gen(level, bounds, path);
    }
    
    std::cout << "Finished" << std::endl;
    return 0;
}