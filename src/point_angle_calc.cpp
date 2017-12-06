
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

float radtodegree_360(float n) {
	float temp = n*(180.0 / CV_PI);

	if (temp < 0)
		return 360 + temp;
	else
		return temp;
}


float radtodegree(float n) {
	float temp = n*(180.0 / CV_PI);

	if (temp < 0)
		return 180 - abs(temp);
	else
		return temp;
}

float dist_Points(Point2f a, Point2f b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}