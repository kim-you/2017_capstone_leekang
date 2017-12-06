#pragma once

#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat get_clear_background(String videoname, int framelimit, Size resolution);
