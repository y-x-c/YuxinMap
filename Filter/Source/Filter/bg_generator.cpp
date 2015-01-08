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
                               pugi::xml_node &bounds, link_t &link, int st_level, int ed_level):
        link(link), st_level(st_level), ed_level(ed_level), osm(osm), conf(conf)
    {
        auto bg_layer = conf.child("layers").find_child_by_attribute("layer", "layer", "background");
        color_land = bg_layer.attribute("color_land").as_string();
        color_sea = bg_layer.attribute("color_sea").as_string();
        method = bg_layer.attribute("method").as_string();
        
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
        bg_nodes.resize(ed_level - st_level + 1);
        
        for(int i = 0; i < bgs.size(); i++)
        {
            if(br_txs[i] < tl_txs[i] || br_tys[i] < tl_tys[i]) continue;
            
            bgs[i].resize(br_txs[i] - tl_txs[i] + 1);
            bg_nodes[i].resize(br_txs[i] - tl_txs[i] + 1);
            
            for(LL j = tl_txs[i]; j <= br_txs[i]; j++)
            {
                bgs[i][j - tl_txs[i]].resize(br_tys[i] - tl_tys[i] + 1, 0);
                bg_nodes[i][j - tl_txs[i]].resize(br_tys[i] - tl_tys[i] + 1);
            }
        }
        
        init_ed_level();
        
        for(int l = int(bgs.size()) - 2; l >= 0; l--) {
            for(int tx = 0; tx < bgs[l].size(); tx++) {
                for(int ty = 0; ty < bgs[l][tx].size(); ty++) {
                    int tx1 = int((tx + tl_txs[l]) * 2 - tl_txs[l + 1]), tx2 = tx1 + 1;
                    int ty1 = int((ty + tl_tys[l]) * 2 - tl_tys[l + 1]), ty2 = ty1 + 1;
                    bool f = (bgs[l + 1][tx1][ty1] == bgs[l + 1][tx1][ty2]) &&
                            (bgs[l + 1][tx1][ty2] == bgs[l + 1][tx2][ty1]) &&
                            (bgs[l + 1][tx2][ty1] == bgs[l + 1][tx2][ty2]);
                    
                    if(f)
                        bgs[l][tx][ty] = bgs[l + 1][tx1][tx2];
                    else {
                        bgs[l][tx][ty] = 1;
                        
                        bg_nodes[l][tx][ty].insert(bg_nodes[l + 1][tx1][ty1].begin(), bg_nodes[l + 1][tx1][ty1].end());
                        bg_nodes[l][tx][ty].insert(bg_nodes[l + 1][tx1][ty2].begin(), bg_nodes[l + 1][tx1][ty2].end());
                        bg_nodes[l][tx][ty].insert(bg_nodes[l + 1][tx2][ty1].begin(), bg_nodes[l + 1][tx2][ty1].end());
                        bg_nodes[l][tx][ty].insert(bg_nodes[l + 1][tx2][ty2].begin(), bg_nodes[l + 1][tx2][ty2].end());
                    }
                }
            }
        }
    }
    
    void BG_Generator::init_ed_level() {
        std::queue<std::pair<int, int> > Q;
        int level = ed_level - st_level;
        
        for(auto &way : osm.children("way")) {
            if(!way.find_child_by_attribute("tag", "v", "coastline")) continue;
            auto &coastline = way;
            //std::string name = getName(way);
            
            LL ltx = -1, lty = -1;
            double lpx = -1, lpy = -1;
            
            for(auto &nd : coastline.children("nd")) {
                auto &node = link[getName(nd)];
                std::string name = getName(nd);
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
                            btys.push_back(std::max(ty1, ty2) - tl_tys[level] - 1);
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
                            btys.push_back(std::max(ty1, ty2) - tl_tys[level] - 1);
                            ttys.push_back(std::min(ty1, ty2) - tl_tys[level]);
                        }
                        
                        for(int i = 0; i < txs.size(); i++) {
                            for(LL j = ttys[i]; j <= btys[i]; j++) {
                                if(!(txs[i] >= 0 && txs[i] <= br_txs[level] - tl_txs[level] &&
                                     j >= 0 && j <= br_tys[level] - tl_tys[level])) continue;
                                
                                bg_nodes[level][txs[i]][j].insert(name);
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
//                            std::cerr << int(level - bgs.size()) << std::endl;
//                            std::cerr << int(pt.tx - tl_txs[level] - bgs[level].size()) << std::endl;
//                            std::cerr << int(ty - tl_tys[level] - bgs[level][pt.tx - tl_txs[level]].size()) << std::endl;
                            
                            bg_nodes[level][pt.tx - tl_txs[level]][ty - tl_tys[level]].insert(name);
                        }
                    }
                }
                
                ltx = pt.tx; lty = pt.ty;
                lpx = pt.px; lpy = pt.py;
            }
        }
        
        for(auto &way : osm.children("way")) {
            if(!way.find_child_by_attribute("tag", "v", "coastline")) continue;
            auto &coastline = way;
            
            LL ltx = -1, lty = -1;
            double lpx = -1, lpy = -1;
            
            for(auto &nd : coastline.children("nd")) {
                auto &node = link[getName(nd)];
                Coord pt(node.attribute("lat").as_double(), node.attribute("lon").as_double(), ed_level);
                
                if(ltx != -1) {
                    
                    if(ltx - pt.tx != 0) {
                        std::vector<LL> ty1s, ty2s, txs;
                        
                        if(pt.tx < ltx) {
                            double k = double(pt.py - lpy) / (pt.px - lpx);
                            double ty1, ty2;
                            
                            ty2 = lpy / tile_l;
                            ty1 = lpy / tile_l - (lpx - ltx * tile_l) / tile_l * k;
                            txs.push_back(ltx - tl_txs[level]);
                            ty1s.push_back(ty1 - tl_tys[level]);
                            ty2s.push_back(ty2 - tl_tys[level]);
                            
                            ty2 = ty1;
                            ty1 = ty2 - k;
                            for(LL tx = ltx - 1; tx >= pt.tx + 1; tx--) {
                                txs.push_back(tx - tl_txs[level]);
                                ty1s.push_back(ty1 - tl_tys[level]);
                                ty2s.push_back(ty2 - tl_tys[level]);
                                
                                ty1 -= k;
                                ty2 -= k;
                            }
                            
                            ty2 = pt.ty;
                            ty1 = (tile_l * (pt.tx + 1) - pt.px) / tile_l * k + pt.py / tile_l;
                            
                            txs.push_back(pt.tx - tl_txs[level]);
                            ty1s.push_back(ty1 - tl_tys[level]);
                            ty2s.push_back(ty2 - tl_tys[level]);

                        } else {
                            double k = double(pt.py - lpy) / (pt.px - lpx);
                            double ty1, ty2;
                            ty1 = lty;
                            ty2 = (tile_l * (ltx + 1) - lpx) / tile_l * k + lpy / tile_l;
                            
                            txs.push_back(ltx - tl_txs[level]);
                            ty1s.push_back(ty1 - tl_tys[level]);
                            ty2s.push_back(ty2 - tl_tys[level]);
                            
                            ty1 = ty2;
                            ty2 = ty1 + k;
                            for(LL tx = ltx + 1; tx < pt.tx; tx++) {
                                txs.push_back(tx - tl_txs[level]);
                                ty1s.push_back(ty1 - tl_tys[level]);
                                ty2s.push_back(ty2 - tl_tys[level]);
                                
                                ty1 += k;
                                ty2 += k;
                            }
                            
                            ty2 = pt.ty;
                            txs.push_back(pt.tx - tl_txs[level]);
                            ty1s.push_back(ty1 - tl_tys[level]);
                            ty2s.push_back(ty2 - tl_tys[level]);
                        }
                        
                        LL _ltx = -1, _lty = -1;
                        
                        for(int i = 0; i < txs.size(); i++) {
                            for(LL j = ty1s[i]; j <= ty2s[i]; j++) {
                                if(!(txs[i] >= 0 && txs[i] <= br_txs[level] - tl_txs[level] &&
                                     j >= 0 && j <= br_tys[level] - tl_tys[level])) continue;
                                
                                if(_ltx != -1) {
                                    LL dtx = (txs[i] - _ltx);
                                    LL dty = (j - _lty);
                                    LL ntx = _ltx - dty;
                                    LL nty = _lty + dtx;
                                    
                                    if(bg_nodes[level][txs[i]][j].size() == 1) {
                                        if(ntx >= 0 && ntx <= br_txs[level] && nty >=0 && nty <= br_tys[level] && bgs[level][ntx][nty] == 0) {
                                            Q.push(std::make_pair(ntx, nty));
                                            bgs[level][ntx][nty] = -1;
                                            
                                            if(ntx + tl_txs[level] == 27132 && nty + tl_tys[level] == 14081)
                                                std::cerr << "here" << std::endl;
                                        }
                                    }
                                }
                                
                                _ltx = txs[i];
                                _lty = j;
                            }
                        }
                        
                    } else if(lty - pt.ty != 0) {
                        LL minty = std::min(lty, pt.ty);
                        LL maxty = std::max(lty, pt.ty);
                        
                        for(LL ty = minty; ty <= maxty; ty++) {
                            if(!(pt.tx >= tl_txs[level] && pt.tx <= br_txs[level] &&
                                 ty >= tl_tys[level] && ty <= br_tys[level])) continue;
                            
                            if(bg_nodes[level][pt.tx - tl_txs[level]][ty - tl_tys[level]].size() == 1) {
                                if (minty == lty) {
                                    if (pt.tx > tl_txs[level] && bgs[level][pt.tx - tl_txs[level] - 1][ty - tl_tys[level]] == 0) {
                                        bgs[level][pt.tx - tl_txs[level] - 1][ty - tl_tys[level]] = -1;
                                        Q.push(std::make_pair(pt.tx - tl_txs[level] - 1, ty - tl_tys[level]));
                                    }
                                } else {
                                    if (pt.tx < br_txs[level] && bgs[level][pt.tx - tl_txs[level] + 1][ty - tl_tys[level]] == 0) {
                                        bgs[level][pt.tx - tl_txs[level] + 1][ty - tl_tys[level]] = -1;
                                        Q.push(std::make_pair(pt.tx - tl_txs[level] + 1, ty - tl_tys[level]));
                                    }
                                }
                            }
                        }
                    }
                }
                
                ltx = pt.tx; lty = pt.ty;
                lpx = pt.px; lpy = pt.py;
            }
        }

        int dx[4] = {0, 0, 1, -1};
        int dy[4] = {1, -1, 0, 0};
        
        while(!Q.empty()) {
            int tx = Q.front().first, ty = Q.front().second;
            Q.pop();
            
            for(int i = 0; i < 4; i++) {
                int ntx = tx + dx[i], nty = ty + dy[i];
                if(ntx >= 0 && ntx <= br_txs[level] - tl_txs[level] && nty >= 0 &&
                   nty <= br_tys[level] - tl_tys[level] && bgs[level][ntx][nty] == 0) {
                     bgs[level][ntx][nty] = -1;
                     Q.push(std::make_pair(ntx, nty));
                }
            }
        }
    }
    
    std::string BG_Generator::get(int level, LL tx, LL ty) {
        std::stringstream ss;
        level -= st_level;
        tx -= tl_txs[level];
        ty -= tl_tys[level];

        if(level == 0 && tx == 2 && ty == 14)
            std::cerr << "ok" << std::endl;
        
        ss << "layer background method background \n1 \nmethod " << method << " ";
        
        if(bgs[level][tx][ty] == -1) {
            ss << "color " << color_sea << " \n0 0 0 1024 1024 1024 1024 0 0 0 ";
        } else if(bgs[level][tx][ty] == 1) {
            ss << "color " << "#00ff00" << " \n0 0 0 1024 1024 1024 1024 0 0 0 ";
        } else {
            ss << "color " << color_land << " \n0 0 0 1024 1024 1024 1024 0 0 0 ";
        }
        
        ss << std::endl;
        
        return ss.str();
    }
    
} // namespace YuxinMap