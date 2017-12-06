#pragma once
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int __inline squre(int A) { return A*A; }
vector<Vec8i> trapezoid_n(Vec8i two_line, Size winSize, int n, float top_ratio);
Vec8i p_lines(Vec8i line, Size winSize, float ratio);
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r);
Rect selectROI(Vec8i pts);
Mat ROI_poly(Size resolution, Vec8i pt);