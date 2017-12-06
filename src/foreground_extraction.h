#pragma once

#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

vector<int> bg_edgeden(int size, Mat src);
vector<Mat> bg_edgeangle(int size, Mat src);
vector<Mat> bg_colorhist(int size, Mat src);
Mat comp_edgeden(int size, Mat src, double mag, vector<int>);
Mat comp_edgeangle(int size, Mat src, double mag, vector<Mat>);
Mat comp_colorhist(int size, Mat src, double mag, vector<Mat>);
Mat mergeMat(Size resolution, Mat den, double denw, Mat angle, double anglew, Mat color, double colorw, int winsize, float threshold);
Mat neighb_filter(Mat src, int ksize, double weight, int winsize);