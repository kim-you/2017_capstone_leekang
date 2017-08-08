#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all
#include "RoadDetection.h"
#include <functional>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation2.jpg", 1);
	Mat src2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation1.jpg", 1);
	Mat src3 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation2_2.jpg", 1);
	Mat src4 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation3_2.jpg", 1);
	
	Mat gray, sobel_x, sobel_y, result;
	
	resize(src, src, Size(400, 300));
	resize(src2, src2, Size(400, 300));
	resize(src3, src3, Size(400, 300));
	resize(src4, src4, Size(400, 300));

	DistHisto(src, src2, src3, src4, 20);
	DistEdgeCompare(src, src2, src3, src4, 20);
	//imshow("ORIGIN", src);

	imshow("A", src);
	imshow("A_1", src2);
	imshow("B", src3);
	imshow("C", src4);

	waitKey(0);

	return 0;

}