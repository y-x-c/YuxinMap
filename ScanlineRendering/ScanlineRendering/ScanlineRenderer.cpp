//
//  ScanlineRenderer.cpp
//  ScanlineRenderer
//
//  Created by 陈裕昕 on 1/9/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include "ScanlineRenderer.h"

ScanlineRenderer::ScanlineRenderer(int width, int height, cv::Scalar bg_color) {
    img = Mat(height, width, CV_8UC3, bg_color);
}

Mat ScanlineRenderer::plot(vector<vector<Point> > &contours, cv::Scalar color) {
    vector<PolyEdge> edges;
    
    for(auto &contour : contours) {
        polylines(img, contour, false, color, 1, CV_AA, 0);
    }
    
    imshow("debug", img);
    waitKey(0);
    
    for(auto &contour : contours) {
        Point pt0 = contour[0];
        pt0.x <<= XY_SHIFT;
        
        for(int i = 1; i < contour.size(); i++) {
            Point pt1 = contour[i];
            pt1.x <<= XY_SHIFT;
            
            if(pt0.y == pt1.y) {
                pt0 = pt1;
                continue;
            }
            
            PolyEdge edge;
            if(pt0.y < pt1.y) {
                edge.t2b = true;
                edge.ty = pt0.y;
                edge.by = pt1.y;
                edge.x = pt0.x;
                edge.dx = (pt1.x - pt0.x) / (pt1.y - pt0.y);
            } else {
                edge.t2b = false;
                edge.ty = pt1.y;
                edge.by = pt0.y;
                edge.x = pt1.x;
                edge.dx = (pt0.x - pt1.x) / (pt0.y - pt1.y);
            }
            edges.push_back(edge);
            
            pt0 = pt1;
        }
    }
    
    sort(edges.begin(), edges.end());
    int total = int(edges.size());
    PolyEdge dummy;
    dummy.ty = INT_MAX;
    edges.push_back(dummy);
    
    uchar _color[3] = {(uchar)color[0], (uchar)color[1], (uchar)color[2]};
    PolyEdge head, *cur = 0, *pre = 0, *e;
    head.next = 0;

    for(int i = 0; i < edges.size(); i++) {
        std::cerr << edges[i].x << " " << edges[i].ty << " " << edges[i].by << " " << edges[i].dx << std::endl;
    }
    
    int idx_edge = 0;
    int yt = edges[0].ty, yb = img.rows;
    e = &edges[idx_edge];
    bool drawHLine = false;
    
    for(int y = yt; y < yb; y++) {
        bool draw = false;
        int count = 0, filled_pixs = 0;
        PolyEdge *keep_pre = 0;
        uchar *timg = img.ptr(y);
        
        pre = &head;
        cur = pre->next;
        while(cur || e->ty == y) {
            if(cur && cur->by == y) {
                pre->next = cur->next;
                cur = cur->next;
                continue;
            }
            
            drawHLine = false;
            keep_pre = pre;
            if(cur && (e->ty > y || e->x > cur->x)) {
                pre = cur;
                cur = cur->next;
            } else if(idx_edge < total) {
                e->next = cur;
                pre->next = e;
                pre = e;
                e = &edges[++idx_edge];
            } else
                break;
            
            if(count++ == 0 && !pre->t2b) {
                if(y >= 0) {
                    int lx = 0, rx = (pre->x >> XY_SHIFT);
                    if(rx >= img.cols) rx = img.cols - 1;
                    
                    if(lx < img.cols && rx >= 0) {
                        filled_pixs += rx - lx + 1;
                        ICV_HLINE(timg, lx, rx, _color, 3);
                    }
                }
                pre->x += pre->dx;
                draw = false;
                continue;
            }
            
            if(draw) {
                if(y >= 0) {
                    int lx, rx;
                    if(keep_pre->x < pre->x) {
                        lx = keep_pre->x;
                        rx = pre->x;
                    } else {
                        lx = pre->x;
                        rx = keep_pre->x;
                    }
                    
                    lx = (lx + XY_ONE - 1) >> XY_SHIFT;
                    rx >>= XY_SHIFT;
                    
                    if(lx < 0) lx = 0;
                    if(rx >= img.cols) rx = img.cols - 1;
                    
                    if(lx < img.cols && rx >= 0) {
                        filled_pixs += rx - lx + 1;
                        ICV_HLINE(timg, lx, rx, _color, 3);
                    }
                }
                keep_pre->x += keep_pre->dx;
                pre->x += pre->dx;
            }
            
            draw ^= 1;
        }
        
        if(draw) {
            if(y >= 0) {
                int lx = (pre->x + XY_ONE - 1) >> XY_SHIFT, rx = img.cols - 1;
                if(lx < 0) lx = 0;
                
                if(lx < img.cols && rx >= 0) {
                    filled_pixs += rx - lx + 1;
                    ICV_HLINE(timg, lx, rx, _color, 3);
                }
            }
            draw = false;
            pre->x += pre->dx;
        }
        
        if(filled_pixs == img.cols) drawHLine = true;
        if(count == 0 && drawHLine && y >= 0) {
            int lx = 0, rx = img.cols - 1;
            ICV_HLINE(timg, lx, rx, _color, 3);
        }
        if(y == edges[0].ty && drawHLine) {
            for(int yy = 0; yy < y; yy++) {
                int lx = 0, rx = img.cols - 1;
                ICV_HLINE(img.ptr(yy), lx, rx, _color, 3);
            }
        }
        
        // opencv
        int sort_flag = 0;
        do
        {
            pre = &head;
            cur = head.next;
            
            while( cur && cur != keep_pre && cur->next != 0 )
            {
                PolyEdge *te = cur->next;
                
                // swap edges
                if( cur->x > te->x )
                {
                    pre->next = te;
                    cur->next = te->next;
                    te->next = cur;
                    pre = te;
                    sort_flag = 1;
                }
                else
                {
                    pre = cur;
                    cur = te;
                }
            }
            keep_pre = pre;
        }
        while( sort_flag && keep_pre != head.next && keep_pre != &head );
    }
    
    imshow("debug", img);
    waitKey(0);
    
    return img;
}