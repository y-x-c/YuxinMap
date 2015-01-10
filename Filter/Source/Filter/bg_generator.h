//
//  bg_generator.h
//  Filter
//
//  Created by 陈裕昕 on 1/5/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#ifndef __Filter__bg_generator__
#define __Filter__bg_generator__

#include <vector>
#include <queue>
#include <set>
#include <string>
#include <sstream>
#include "pugixml.hpp"
#include "utility.h"
#include "ScanlineRenderer.h"
#include <opencv2/opencv.hpp>

namespace YuxinMap {

    class BG_Generator {
    private:
        link_t &link;
        
        double min_lat, max_lat, min_lon, max_lon;
        std::vector<LL> tl_txs, tl_tys, br_txs, br_tys;
        std::string color_land, color_sea;
        std::string path;
        cv::Scalar color_land_s, color_sea_s;
        
        std::vector<std::vector<std::vector<std::set<std::string> > > > bg_ways;
        std::vector<std::vector<std::vector<int> > > bgs;
        pugi::xml_node osm, conf;
        int st_level, ed_level;

        void init();
        void init_bg_ways();
        void plot(std::set<std::string> &ways, LL tx, LL ty, int level);
        void plot_all();
    public:
        BG_Generator(pugi::xml_node &osm, pugi::xml_node &conf, pugi::xml_node &bounds, link_t &link, int st_level, int ed_level, std::string path);
        
        std::string get(int level, LL tx, LL ty);
    };
    
} // namespace YuxinMap

#endif /* defined(__Filter__bg_generator__) */
