#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "RoadDetection.h"

using namespace std;
using namespace cv;

//Mat DistHisto(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE);

int main()
{
	int sample = 0;
	int match0 = 0;
	int match1 = 0;
	int match2 = 0;

	Mat src_base, hsv_base, hsv_base1;
	Mat src_test1, hsv_test1;
	Mat src_test2, hsv_test2;
	Mat src_line;


	src_base = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation2.jpg", 1);
	Mat src_base1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation1.jpg", 1);
	src_test1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation2_2.jpg", 1);
	src_test2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation3_1.jpg", 1);


	Mat Mask1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Mask1.jpg", CV_8UC1);
	Mat Mask2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Mask2.jpg", CV_8UC1);
	Mat Mask3 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Mask3.jpg", CV_8UC1);


	resize(src_base, src_base, Size(400, 300));
	resize(src_base1, src_base1, Size(400, 300));
	resize(src_test1, src_test1, Size(400, 300));
	resize(src_test2, src_test2, Size(400, 300));

	resize(Mask1, Mask1, Size(400, 300));
	resize(Mask2, Mask2, Size(400, 300));
	resize(Mask3, Mask3, Size(400, 300));

	Mask1 = Mask1 > 128;
	Mask2 = Mask2 > 128;
	Mask3 = Mask3 > 128;

	Mat test_mask;

	DistHisto(src_base, src_base1, src_test1, src_test2, 20);

	imshow("BASE", src_base);
	imshow("A", src_base1);
	imshow("B", src_test1);
	imshow("C", src_test2);

	printf("Done \n");
	
	//imshow("MASK_TEST", test_mask);

	waitKey(0);

	return 0;

}
