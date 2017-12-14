#pragma once
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat calcVec(String &filename, Size &resolution, int &grid_size, double &threshold, Mat &road_Map, int &check_finish, double &size_ratio, double &forgive_ratio, double &non_ratio);
vector<Vec4i> findRoadLine(Mat roadBlob, Mat filtered);
vector<Vec4i> externalLine(vector<Vec4i> lines, Size resolution, Mat roadblob);
vector<Vec4i> extendLine(vector<Vec4i> lines, Size resolution);
bool lineDecision(Vec4i line, int gridSize, Mat angleMap, float decision_ratio, float angle_range);
bool comp(const Point2f a, const Point2f b);