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
#include "bg_generator.h"
#include "utility.h"

int main(int argc, const char * argv[]) {
    std::ios::sync_with_stdio(false);
    
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
    std::cout << "Plot conf load result: " << result_plot_conf.description() << std::endl;
    if(!result_plot_conf) return -3;
    pugi::xml_node bounds = osm.child("bounds");
    pugi::xml_node plot_conf = plot_conf_doc.child("YuxinMap");
    std::string path(argv[4]);
    
    int min_level = atoi(argv[5]), max_level = atoi(argv[6]);

    std::ofstream dt_list(path + "dt_list", std::ofstream::out);
    dt_list.close();

    YuxinMap::BG_Generator bg_generator(osm, plot_conf, bounds, link, min_level, max_level);
    
    for(int level = min_level; level <= max_level; level++)
    {
        std::cout << "Level " << level << std::endl;
        std::stringstream ss;
        ss << level;
        level_filter.filter(conf.find_child_by_attribute("elements", "level", ss.str().c_str()), ac);
    
        YuxinMap::Data_Converter data_converter(level, bounds, link, ac, plot_conf, bg_generator);
        data_converter.convert(path, elem_layers, elem_plot_methods);
    }
    
    std::cout << "Finished" << std::endl;
    return 0;
}