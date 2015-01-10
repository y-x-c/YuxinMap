//
//  bg_generator.cpp
//  Filter
//
//  Created by 陈裕昕 on 1/5/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include "bg_generator.h"

namespace YuxinMap {
    
    BG_Generator::BG_Generator(pugi::xml_node &osm, pugi::xml_node &conf,
                               pugi::xml_node &bounds, link_t &link, int st_level, int ed_level, std::string path):
        link(link), st_level(st_level), ed_level(ed_level), osm(osm), conf(conf), path(path)
    {
        auto bg_layer = conf.child("layers").find_child_by_attribute("layer", "layer", "background");
        color_land = bg_layer.attribute("color_land").as_string();
        color_sea = bg_layer.attribute("color_sea").as_string();
        
        color_land_s = getScalar(color_land);
        color_sea_s = getScalar(color_sea);
        
        for(int i = 0; i < 3; i++) color_sea_u[i] = color_sea_s[i];
        for(int i = 0; i < 3; i++) color_land_u[i] = color_land_s[i];
        
        min_lat = bounds.attribute("minlat").as_double();
        max_lat = bounds.attribute("maxlat").as_double();
        min_lon = bounds.attribute("minlon").as_double();
        max_lon = bounds.attribute("maxlon").as_double();
        
        for(int level = st_level; level <= ed_level; level++)
        {
            Coord tl(max_lat, min_lon, level), br(min_lat, max_lon, level);
            LL tl_tx = tl.tx, tl_ty = tl.ty;
            LL br_tx = br.tx, br_ty = br.ty;
            
            LL _x, _y;
            tl.get_x_y(_x, _y);
            if(_x != 0) tl_tx++;
            if(_y != 0) tl_ty++;
            
            br.get_x_y(_x, _y);
            if(_x != tile_l - 1) br_tx--;
            if(_y != tile_l - 1) br_ty--;
            
            tl_txs.push_back(tl_tx);
            tl_tys.push_back(tl_ty);
            br_txs.push_back(br_tx);
            br_tys.push_back(br_ty);
        }
        
        init();
    }
    
    void BG_Generator::init() {
        bgs.resize(ed_level - st_level + 1);
        bg_ways.resize(ed_level - st_level + 1);
        
        for(int i = 0; i < bgs.size(); i++)
        {
            if(br_txs[i] < tl_txs[i] || br_tys[i] < tl_tys[i]) continue;
            
            bgs[i].resize(br_txs[i] - tl_txs[i] + 1);
            bg_ways[i].resize(br_txs[i] - tl_txs[i] + 1);
            
            for(LL j = tl_txs[i]; j <= br_txs[i]; j++)
            {
                bgs[i][j - tl_txs[i]].resize(br_tys[i] - tl_tys[i] + 1, 0);
                bg_ways[i][j - tl_txs[i]].resize(br_tys[i] - tl_tys[i] + 1);
            }
        }
        
        init_bg_ways();
        plot_all();
    }
    
    void BG_Generator::init_bg_ways() {
        int level = ed_level - st_level;
        has_coastline = false;
        
        for(auto &way : osm.children("way")) {
            if(!way.find_child_by_attribute("tag", "v", "coastline")) continue;
            auto &coastline = way;
            std::string name = getName(way);
            
            LL ltx = -1, lty = -1;
            double lpx = -1, lpy = -1;
            
            for(auto &nd : coastline.children("nd")) {
                has_coastline = true;
                
                auto &node = link[getName(nd)];
                Coord pt(node.attribute("lat").as_double(), node.attribute("lon").as_double(), ed_level);
                
                if(ltx != -1) {

                    if(ltx - pt.tx != 0) {
                        std::vector<LL> btys, ttys, txs;
                        
                        if(pt.tx < ltx) {
                            double k = double(pt.py - lpy) / (pt.px - lpx);
                            double ty1, ty2;
                            ty1 = pt.ty;
                            ty2 = (tile_l * (pt.tx + 1) - pt.px) / tile_l * k + pt.py / tile_l;
                            
                            txs.push_back(pt.tx - tl_txs[level]);
                            btys.push_back(std::max(ty1, ty2) - tl_tys[level]);
                            ttys.push_back(std::min(ty1, ty2) - tl_tys[level]);
                            
                            ty1 = ty2;
                            ty2 = ty1 + k;
                            for(LL tx = pt.tx + 1; tx < ltx; tx++) {
                                txs.push_back(tx - tl_txs[level]);
                                btys.push_back(std::max(ty1, ty2) - tl_tys[level]);
                                ttys.push_back(std::min(ty1, ty2) - tl_tys[level]);
                                
                                ty1 += k;
                                ty2 += k;
                            }
                            
                            ty2 = lty;
                            txs.push_back(ltx - tl_txs[level]);
                            btys.push_back(std::max(ty1, ty2) - tl_tys[level]);
                            ttys.push_back(std::min(ty1, ty2) - tl_tys[level]);
                        } else {
                            double k = double(pt.py - lpy) / (pt.px - lpx);
                            double ty1, ty2;
                            ty1 = lty;
                            ty2 = (tile_l * (ltx + 1) - lpx) / tile_l * k + lpy / tile_l;
                            
                            txs.push_back(ltx - tl_txs[level]);
                            btys.push_back(std::max(ty1, ty2) - tl_tys[level]);
                            ttys.push_back(std::min(ty1, ty2) - tl_tys[level]);
                            
                            ty1 = ty2;
                            ty2 = ty1 + k;
                            for(LL tx = ltx + 1; tx < pt.tx; tx++) {
                                txs.push_back(tx - tl_txs[level]);
                                btys.push_back(std::max(ty1, ty2) - tl_tys[level]);
                                ttys.push_back(std::min(ty1, ty2) - tl_tys[level]);
                                
                                ty1 += k;
                                ty2 += k;
                            }
                            
                            ty2 = pt.ty;
                            txs.push_back(pt.tx - tl_txs[level]);
                            btys.push_back(std::max(ty1, ty2) - tl_tys[level]);
                            ttys.push_back(std::min(ty1, ty2) - tl_tys[level]);
                        }
                        
                        for(int i = 0; i < txs.size(); i++) {
                            for(LL j = ttys[i]; j <= btys[i]; j++) {
                                if(!(txs[i] >= 0 && txs[i] <= br_txs[level] - tl_txs[level] &&
                                     j >= 0 && j <= br_tys[level] - tl_tys[level])) continue;
                                
                                bg_ways[level][txs[i]][j].insert(name);
                                bgs[level][txs[i]][j] = 1;
                            }
                        }

                    } else {
                        LL minty = std::min(lty, pt.ty);
                        LL maxty = std::max(lty, pt.ty);
                        for(LL ty = minty; ty <= maxty; ty++) {
                            if(!(pt.tx >= tl_txs[level] && pt.tx <= br_txs[level] &&
                                 ty >= tl_tys[level] && ty <= br_tys[level])) continue;
                            
                            bgs[level][pt.tx - tl_txs[level]][ty - tl_tys[level]] = 1;
                            bg_ways[level][pt.tx - tl_txs[level]][ty - tl_tys[level]].insert(name);
                        }
                    }
                }
                
                ltx = pt.tx; lty = pt.ty;
                lpx = pt.px; lpy = pt.py;
            }
        }
        
        for(int l = level - 1; l >= 0; l--) {
            for(int tx = 0; tx < bgs[l].size(); tx++) {
                for(int ty = 0; ty < bgs[l][tx].size(); ty++) {
                    int tx1 = int((tx + tl_txs[l]) * 2 - tl_txs[l + 1]), tx2 = tx1 + 1;
                    int ty1 = int((ty + tl_tys[l]) * 2 - tl_tys[l + 1]), ty2 = ty1 + 1;
                    
                    bg_ways[l][tx][ty].insert(bg_ways[l + 1][tx1][ty1].begin(), bg_ways[l + 1][tx1][ty1].end());
                    bg_ways[l][tx][ty].insert(bg_ways[l + 1][tx1][ty2].begin(), bg_ways[l + 1][tx1][ty2].end());
                    bg_ways[l][tx][ty].insert(bg_ways[l + 1][tx2][ty1].begin(), bg_ways[l + 1][tx2][ty1].end());
                    bg_ways[l][tx][ty].insert(bg_ways[l + 1][tx2][ty2].begin(), bg_ways[l + 1][tx2][ty2].end());
                    
                    bgs[l][tx][ty] = (bg_ways[l][tx][ty].size() > 0);
                }
            }
        }
    }
    
    void BG_Generator::plot(std::set<std::string> &ways, LL tx, LL ty, int level) {
        int rtile_l = retina_factor * sampling_factor * tile_l;
        std::vector<std::vector<cv::Point> > contours;
        
        for(auto &way_name : ways) {
            auto way = link[way_name];
            
            contours.push_back(std::vector<cv::Point>());
            std::vector<cv::Point> &pts = contours[contours.size() - 1];
            
            for(auto &nd : way.children("nd")) {
                auto &node = link[getName(nd)];
                Coord pt(node.attribute("lat").as_double(), node.attribute("lon").as_double(), level);
                LL _x, _y;
                pt.get_x_y(_x, _y, tx, ty);
                int x = int(_x), y = int(_y);

                pts.push_back(cv::Point(x, y));
            }
        }
        
        ScanlineRenderer renderer(rtile_l, rtile_l, color_sea_s);
        cv::Mat ret = renderer.plot(contours, color_land_s);
        cv::resize(ret, ret, cv::Size(rtile_l, rtile_l));
        
        std::stringstream ss;
        ss << path << tx << "_" << ty << "_" << level << "_bg.png";
        
        cv::imwrite(ss.str(), ret);
        
        uchar *st_ptr[4] = {ret.ptr(0), ret.ptr(rtile_l - 1), ret.ptr(0, rtile_l - 1), ret.ptr(0)};
        int step[4] = {3, 3, 3 * rtile_l, 3 * rtile_l};
        int dx[4] = {0, 0, 1, -1};
        int dy[4] = {-1, 1, 0, 0};
        int l = level - st_level;
        
        for(int j = 0; j < 4; j++) {
            int i = 0;
            bool flag;
            uchar *ptr = st_ptr[j];
            
            uchar color0[3] = {ptr[0], ptr[1], ptr[2]};
            ptr += step[j]; i++;
            
            for(flag = true; flag && i < rtile_l; i++) {
                for(int k = 0; k < 3; k++) {
                    if(color0[k] != ptr[k]) {
                        flag = false;
                        break;
                    }
                }
                ptr += step[j];
            }
            
            if(flag) {
                LL ntx = tx + dx[j] - tl_txs[l], nty = ty + dy[j] - tl_tys[l];
                
                if(ntx >= 0 && ntx <= br_txs[l] - tl_txs[l] && nty >= 0 &&
                   nty <= br_tys[l] - tl_tys[l] && bgs[l][ntx][nty] == 0) {
                    Q.push(std::make_pair(ntx, nty));
                    if(color0[0] == color_land_u[0] && color0[1] == color_land_u[1] && color0[2] == color_land_u[2]) {
                        bgs[l][ntx][nty] = -1; // land
                    } else {
                        bgs[l][ntx][nty] = -2; // water
                    }
                }
            }
        }
    }
    
    void BG_Generator::plot_all() {
        int l_ed = ed_level - st_level;
        for(int tx = 0; tx < bg_ways[l_ed].size(); tx++) {
            for(int ty = 0; ty < bg_ways[l_ed][tx].size(); ty++) {
                if(bg_ways[l_ed][tx][ty].size()) {
                    std::string output_path;
                    plot(bg_ways[l_ed][tx][ty], tx + tl_txs[l_ed], ty + tl_tys[l_ed], ed_level);
                }
            }
        }
        
        int dx[4] = {0, 0, 1, -1};
        int dy[4] = {1, -1, 0, 0};
        if(!has_coastline) Q.push(std::make_pair(0, 0));
        bgs[l_ed][0][0] = -1;
        while(!Q.empty()) {
            int tx = Q.front().first, ty = Q.front().second;
            Q.pop();

            for(int i = 0; i < 4; i++) {
                int ntx = tx + dx[i], nty = ty + dy[i];
                if(ntx >= 0 && ntx <= br_txs[l_ed] - tl_txs[l_ed] && nty >= 0 &&
                   nty <= br_tys[l_ed] - tl_tys[l_ed] && bgs[l_ed][ntx][nty] == 0) {
                     bgs[l_ed][ntx][nty] = bgs[l_ed][tx][ty];
                     Q.push(std::make_pair(ntx, nty));
                }
            }
        }

        int rtile_l = retina_factor * sampling_factor * tile_l;
        cv::Mat sea(rtile_l, rtile_l, CV_8UC3, color_sea_s);
        cv::Mat land(rtile_l, rtile_l, CV_8UC3, color_land_s);
        
        int l = ed_level - st_level;
        for(int tx = 0; tx < bg_ways[l].size(); tx++) {
            for(int ty = 0; ty < bg_ways[l][tx].size(); ty++) {
                std::stringstream ss;
                ss << path << tx + tl_txs[l] << "_" << ty + tl_tys[l] << "_" << l + st_level << "_bg.png";
                
                if(bgs[l][tx][ty] == -1) {
                    cv::imwrite(ss.str(), land);
                }
                if(bgs[l][tx][ty] == -2) {
                    cv::imwrite(ss.str(), sea);
                }
            }
        }

        for(int l = int(bg_ways.size()) - 2; l >= 0; l--) {
            for(int tx = 0; tx < bg_ways[l].size(); tx++) {
                for(int ty = 0; ty < bg_ways[l][tx].size(); ty++) {
                    int tx1 = tx + tl_txs[l];
                    int ty1 = ty + tl_tys[l];
                    int txs[4] = {tx1 << 1, tx1 << 1, (tx1 << 1) | 1, (tx1 << 1) | 1};
                    int tys[4] = {ty1 << 1, (ty1 << 1) | 1, ty1 << 1, (ty1 << 1) | 1};
                    cv::Mat img[4];
                    
                    for(int i = 0; i < 4; i++) {
                        std::stringstream ss;
                        ss << path << txs[i] << "_" << tys[i] << "_" << l + 1 + st_level << "_bg.png";
                        img[i] = imread(ss.str());
                        cv::resize(img[i], img[i], cv::Size(rtile_l >> 1, rtile_l >> 1));
                    }
                    
                    cv::Mat ret(rtile_l, rtile_l, CV_8UC3);
                    img[0].copyTo(ret.rowRange(0, rtile_l >> 1).colRange(0, rtile_l >> 1));
                    img[1].copyTo(ret.rowRange(rtile_l >> 1, rtile_l).colRange(0, rtile_l >> 1));
                    img[2].copyTo(ret.rowRange(0, rtile_l >> 1).colRange(rtile_l >> 1, rtile_l));
                    img[3].copyTo(ret.rowRange(rtile_l >> 1, rtile_l).colRange(rtile_l >> 1, rtile_l));
                    
                    std::stringstream ss;
                    ss << path << tx + tl_txs[l] << "_" << ty + tl_tys[l] << "_" << l + st_level << "_bg.png";
                    cv::imwrite(ss.str(), ret);
                }
            }
        }
    }
    
    std::string BG_Generator::get(int level, LL tx, LL ty) {
        std::stringstream ss;
        
        ss << "layer background method background \n";
        ss << path << tx << "_" << ty << "_" << level << "_bg.png \n";
        
        return ss.str();
    }
    
} // namespace YuxinMap