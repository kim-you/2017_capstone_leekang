#pragma once

#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

vector<int> bg_edgeden(int &size, Mat &src);
vector<Mat> bg_edgeangle(int &size, Mat &src);
vector<Mat> bg_colorhist(int &size, Mat &src);
void comp_edgeden(Mat &output, int &size, Mat &src, Size &resolution, double &mag, vector<int>& BGedgenum, Mat &roadMask, Rect &r, Mat &temp);
void comp_edgeangle(Mat &output, int &size, Mat &src, Size &resolution, double &mag, vector<Mat>& histBGangle, Mat &roadMask, Rect &r, Mat &temp, Mat &hist);
void comp_colorhist(Mat &output, int &size, Mat &src, Size &resolution, double &mag, vector<Mat>& histBGcolor, Mat &roadMask, Rect &r, Mat &temp, Mat &hist);
void mergeMat(Mat &output, Size &resolution, Mat &den, double &denw, Mat &angle, double &anglew, Mat &color, double &colorw, int &winsize, double &threshold, Rect &r, Mat &total, Mat &filter);
Mat neighb_filter(Mat &src, int ksize, double weight, int &winsize);