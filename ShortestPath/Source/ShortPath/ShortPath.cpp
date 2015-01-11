//
//  ShortPath.cpp
//  ShortPath
//
//  Created by 陈裕昕 on 1/3/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include "ShortPath.h"

ShortPath::ShortPath(const std::string &osmPath, const std::string &confPath, int c): C(c)
{
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

bool ShortPath::check(const pugi::xml_node &elem, const pugi::xml_node &conf)
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

int ShortPath::getWayClass(const pugi::xml_node &way, double &speed)
{
    int ret = 0;
    pugi::xml_node oneway = way.find_child_by_attribute("tag", "k", "oneway");
    if(oneway.attribute("v").as_bool()) ret |= 1;
    
    for(auto &c : conf.children())
    {
        int ic = c.attribute("class").as_int();
        if(ic != C) continue;
        
        for(auto &filter : c.children("way"))
        {
            if(check(way, filter))
            {
                ret |= ic;
                speed = filter.attribute("speed").as_double();
                break;
            }
        }
    }
    
    return ret;
}

double ShortPath::getDist(int x, int y)
{
    return sqrt(pow(pts[x].lat - pts[y].lat, 2) + pow(pts[x].lon - pts[y].lon, 2));
}

void ShortPath::addedges(int x, int y, int c, double speed)
{
    double time = getDist(x, y) * 100 / speed;
    
    // the way is bidirection if c is odd
    edges[x].push_back(Edge(y, c, time));
    
    if((c & 1) == 0)
    {
        edges[y].push_back(Edge(x, c, time));
    }
}

void ShortPath::init()
{
    for(auto &node : osm.children("node"))
    {
        osmid2id[node.attribute("id").as_string()] = int(pts.size());
        
        double lat = node.attribute("lat").as_double();
        double lon = node.attribute("lon").as_double();
        
        Coord coord(lat, lon);
        pts.push_back(coord);
    }
        
    edges.resize(pts.size());
    
    for(auto &way : osm.children("way"))
    {
        double speed;
        int c = getWayClass(way, speed);
        if(c <= 1) continue;
        
        int firstId = -1, lastId = -1;
        for(auto &nd : way.children("nd"))
        {
            int id = osmid2id[nd.attribute("ref").as_string()];
            if(firstId < 0) firstId = id;
            
            if(lastId >= 0) addedges(lastId, id, c, speed);
            lastId = id;
        }
        
        //if(firstId != lastId) addedges(firstId, lastId, c);
    }
}

void ShortPath::findShortPath(int S, int T, std::vector<int> &path)
{
    std::vector<double> dist(pts.size(), INF);
    std::vector<int> pre(pts.size(), -1);
    std::priority_queue<HeapElem> heap;
    
    std::vector<double> dist2T;
    dist2T.resize(pts.size());
    for(int i = 0; i < pts.size(); i++) dist2T[i] = getDist(i, T);
    
    dist[S] = 0;
    heap.push(HeapElem(S, 0, dist2T[S]));
    
    while(!heap.empty())
    {
        HeapElem top = heap.top();
        heap.pop();
        
        if(top.x == T) break;
        
        if(dist[top.x] != top.d) continue;
        
        for(auto &edge : edges[top.x])
        {
            //if(!(edge.c & C)) continue;
            
            double ndist = dist[top.x] + edge.d;
            if(ndist < dist[edge.y])
            {
                dist[edge.y] = ndist;
                pre[edge.y] = top.x;
                
                double nf = ndist + dist2T[top.x];
                heap.push(HeapElem(edge.y, ndist, nf));
            }
        }
    }
    
    path.clear();
    if(pre[T] == -1) return;
    
    for(int x = T; x != -1; x = pre[x])
    {
        path.push_back(x);
    }
    
    std::reverse(path.begin(), path.end());
}

void ShortPath::walking(std::string _S, std::string _T, const std::string &outputPath)
{
    std::vector<int> ret;
    int S = osmid2id[_S], T = osmid2id[_T];
    
    findShortPath(S, T, ret);
    
    output(ret, outputPath);
}

void ShortPath::driving(std::string _S, std::string _T, const std::string &outputPath)
{
    std::vector<int> ret;
    int S = osmid2id[_S], T = osmid2id[_T];
    
    findShortPath(S, T, ret);
    
    output(ret, outputPath);
}

void ShortPath::output(const std::vector<int> &ret, const std::string &outputPath)
{
    std::ofstream out(outputPath.c_str(), std::ofstream::out);
    
    out << ret.size() << std::endl;
    
    out.precision(10);
    
    for(auto &id : ret)
    {
        out << pts[id].lat << " " << pts[id].lon << " ";
    }
    
    out << std::endl;
}