//
//  main.cpp
//  ScanlineRendering
//
//  Created by 陈裕昕 on 1/9/15.
//  Copyright (c) 2015 Fudan. All rights reserved.
//

#include <iostream>
#include <vector>
#include "ScanlineRenderer.h"

using namespace std;

int main(int argc, const char * argv[]) {
    vector<vector<cv::Point> > contours;

    freopen("input13.txt", "r", stdin);
    int N;
    cin >> N;
    for(int i = 0; i < N; i++) {
        contours.push_back(vector<cv::Point>());
        
        int n;
        cin >> n;
        for(int j = 0; j < n; j++) {
            int x, y;
            cin >> x >> y;
            contours[i].push_back(Point(x, y));
        }
    }
    
    ScanlineRenderer renderer(500, 500, cv::Scalar(254, 189, 150));
    renderer.plot(contours, cv::Scalar(240, 245, 247));
    
    return 0;
}
