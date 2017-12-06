#pragma once

#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat calNonedgeArea(Mat src, int windowSize, float stride, float edgeRate);
Mat findRoadBlob(Mat nonEdgeMap, Mat filtered, float sky_rate, float labLimit, float sigma, float sizeLimit);
