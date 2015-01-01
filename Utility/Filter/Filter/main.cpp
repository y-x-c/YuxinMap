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
#include "pugixml.hpp"
#include "level_filter.h"
#include "data_converter.h"
#include "utility.h"

int main(int argc, const char * argv[]) {
    std::ios::sync_with_stdio(false);
    
    std::cout << argv[1] << std::endl;

    pugi::xml_document osm_doc;
    pugi::xml_parse_result result_osm = osm_doc.load_file(argv[1]);
    std::cout << "OSM load result: " << result_osm.description() << std::endl;
    if(!result_osm) return -1;
    
    pugi::xml_document conf_doc;
    pugi::xml_parse_result result_conf = conf_doc.load_file(argv[2]);
    std::cout << "Conf load result: " << result_conf.description() << std::endl;
    if(!result_conf) return -2;
    
    pugi::xml_node osm = osm_doc.child("osm");
    pugi::xml_node conf = conf_doc.child("YuxinMap");
    
    YuxinMap::link_t link;
    
    for(auto &node : osm.children())
    {
        link[YuxinMap::getName(node)] = node;
    }
    
    YuxinMap::Level_Filter level_filter(osm);
    
    YuxinMap::ac_t ac;
    YuxinMap::elem_layers_t elem_layers;
    YuxinMap::elem_plot_methods_t elem_plot_methods;
    
    pugi::xml_document plot_conf_doc;
    pugi::xml_parse_result result_plot_conf = plot_conf_doc.load_file(argv[3]);
    if(!result_plot_conf) return -3;
    pugi::xml_node bounds = osm.child("bounds");
    pugi::xml_node plot_conf = plot_conf_doc.child("YuxinMap");
    std::string path(argv[4]);
    
    for(int level = 5; level < 18; level++)
    {
        std::stringstream ss;
        ss << level;
        level_filter.filter(conf.find_child_by_attribute("elements", "level", ss.str().c_str()), ac);
    
        YuxinMap::Data_Converter data_converter(level, bounds, link, ac, plot_conf);
        data_converter.convert(path, elem_layers, elem_plot_methods);
    }
    
    return 0;
}