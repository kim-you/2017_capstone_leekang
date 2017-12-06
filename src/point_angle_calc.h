#pragma once
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


float radtodegree_360(float n);
float radtodegree(float);
float dist_Points(Point2f, Point2f);