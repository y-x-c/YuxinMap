//
//  NearestNode.cpp
//  NearestNode
//
//  Created by 陈裕昕 on 1/4/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include "NearestNode.h"

NearestNode::NearestNode(const std::string &osmPath, const std::string &confPath) {

    pugi::xml_parse_result result_osm = osm_doc.load_file(osmPath.c_str());
    std::cout << "OSM load result: " << result_osm.description() << std::endl;
    if(!result_osm) throw ;
    
    osm = osm_doc.child("osm");
    
    pugi::xml_parse_result result_conf = conf_doc.load_file(confPath.c_str());
    std::cout << "Conf load result: " << result_conf.description() << std::endl;
    if(!result_conf) throw ;
    
    conf = conf_doc.child("YuxinMap");
    
    init();
}

void NearestNode::init()
{
    for(const auto &node : osm.children("node"))
    {
        osmid2id[node.attribute("id").as_string()] = int(pts.size());
        id2osmid[int(pts.size())] = node.attribute("id").as_string();
        
        double lat = node.attribute("lat").as_double();
        double lon = node.attribute("lon").as_double();
        
        Coord coord(lat, lon);
        pts.push_back(coord);
    }
}

bool NearestNode::check(const pugi::xml_node &elem, const pugi::xml_node &conf)
{
    // name should be check in advance
    
    if(elem.name() != std::string("way")) {
        // attributes
        for(auto &attr_conf : conf.attributes())
        {
            if(strcmp(elem.attribute(attr_conf.name()).value(), attr_conf.value()) != 0)
            {
                return false;
            }
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

int NearestNode::getWayClass(const pugi::xml_node &way, int c0)
{
    int ret = 0;
    pugi::xml_node oneway = way.find_child_by_attribute("tag", "k", "oneway");
    if(oneway.attribute("v").as_bool()) ret |= 1;
    
    for(auto &c : conf.children())
    {
        int ic = c.attribute("class").as_int();
        if(ic != c0) continue;
        for(auto &filter : c.children("way"))
        {
            if(check(way, filter))
            {
                ret |= ic;
                break;
            }
        }
    }
    
    return ret;
}

std::string NearestNode::find(double lat, double lon, int c0) {
    std::vector<bool> flags;
    
    if(c0 == -1)
    {
        flags.resize(pts.size(), 1);
    }
    else
    {
        flags.resize(pts.size(), 0);
        
        for(auto &way : osm.children("way"))
        {
            int c = getWayClass(way, c0);
            if(!((c & c0) >> 1)) continue;
            
            for(auto &nd : way.children("nd"))
            {
                int id = osmid2id[nd.attribute("ref").as_string()];
                flags[id] = true;
            }
        }
    }
    
    Coord coord0(lat, lon);
    double minDist = 1e60;
    int minId = 0;
    
    for(int id = 0; id < pts.size(); id++) {
        if(!flags[id]) continue;
        
        double dist = coord0.getDist(pts[id]);
        if(dist < minDist) {
            minId = id;
            minDist = dist;
        }
    }
    
    return id2osmid[minId];
}