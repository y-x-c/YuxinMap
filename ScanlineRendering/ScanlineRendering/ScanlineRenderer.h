//
//  ScanlineRenderer.h
//  ScanlineRenderer
//
//  Created by 陈裕昕 on 1/9/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#ifndef __ScanlineRenderer__ScanlineRenderer__
#define __ScanlineRenderer__ScanlineRenderer__

#include <opencv2/opencv.hpp>
#include <iostream>
#include <climits>

using namespace std;
using namespace cv;

// opencv
/* helper macros: filling horizontal row */
#define ICV_HLINE( ptr, xl, xr, color, pix_size )            \
{                                                            \
    uchar* hline_ptr = (uchar*)(ptr) + (xl)*(pix_size);      \
    uchar* hline_max_ptr = (uchar*)(ptr) + (xr)*(pix_size);  \
                                                             \
    for( ; hline_ptr <= hline_max_ptr; hline_ptr += (pix_size))\
    {                                                        \
        int hline_j;                                         \
        for( hline_j = 0; hline_j < (pix_size); hline_j++ )  \
        {                                                    \
            hline_ptr[hline_j] = ((uchar*)color)[hline_j];   \
        }                                                    \
    }                                                        \
}

class PolyEdge {
public:
    int ty, by; // top, bottom
    int x, dx;  // x - coord of top point, delta x
    bool t2b;   // indicate the line's original direction
    
    PolyEdge *next;
    
    bool operator<(const PolyEdge& e) const
    {
        return ty - e.ty ? ty < e.ty :
        x - e.x ? x < e.x : dx < e.dx;
    }
};

class ScanlineRenderer {
private:
    const int XY_SHIFT = 16;
    const int XY_ONE = 1 << XY_SHIFT;
    
    Mat img;
public:
    ScanlineRenderer(int width, int height, Scalar bg_color);
    ~ScanlineRenderer(){}
    
    Mat plot(vector<vector<Point> > &contours, cv::Scalar color);
};

#endif /* defined(__ScanlineRenderer__ScanlineRenderer__) */
