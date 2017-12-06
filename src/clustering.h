#pragma once

#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

vector<Mat> Kmeans(Mat flow, int K);
vector<Mat> del_background_mask(vector<Mat> org, Mat flow);
Mat segment_roabBlob(Mat roadblob, Mat segment);
Mat angle_to_HSV(Mat angleMap, int gridsize, Size resolution);