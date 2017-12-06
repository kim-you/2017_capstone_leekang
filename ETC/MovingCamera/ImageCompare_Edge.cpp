#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


using namespace std;
using namespace cv;

#define SEG_SIZE 10

int main()
{

	int match1 = 0;
	int match2 = 0;

	Mat src_base, hsv_base, hsv_base1;
	Mat src_test1, hsv_test1;
	Mat src_test2, hsv_test2;
	Mat hsv_half_down;
	Mat a, b, c, d;
	Mat aa, bb, cc, dd;
	src_base = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation3.jpg", 1);
	Mat src_base1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation1_1.jpg", 1);
	src_test1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation2_1.jpg", 1);
	src_test2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation3_1.jpg", 1);
	Mat Mask1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Mask1.jpg", CV_8UC1);
	Mat Mask2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Mask2.jpg", CV_8UC1);
	Mat Mask3 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Mask3.jpg", CV_8UC1);


	resize(src_base, src_base, Size(410, 290));
	resize(src_base1, src_base1, Size(410, 290));
	resize(src_test1, src_test1, Size(410, 290));
	resize(src_test2, src_test2, Size(410, 290));
	resize(Mask1, Mask1, Size(410, 290));
	resize(Mask2, Mask2, Size(410, 290));
	resize(Mask3, Mask3, Size(410, 290));

	Mask1 = Mask1 > 128;
	Mask2 = Mask2 > 128;
	Mask3 = Mask3 > 128;

	imshow("M", Mask1);
	imshow("M2", Mask2);
	imshow("M3", Mask3);


	src_base1.copyTo(a, Mask1);
	src_test1.copyTo(b, Mask2);
	src_test2.copyTo(c, Mask3);


	cvtColor(a, a, CV_BGR2GRAY);
	cvtColor(b, b, CV_BGR2GRAY);
	cvtColor(c, c, CV_BGR2GRAY);
	Canny(a, a, 200, 300);
	Canny(b, b, 200, 300);
	Canny(c, c, 200, 300);
	src_base.copyTo(d, Mask3);
	cvtColor(d, d, CV_BGR2GRAY);
	Canny(d, d, 200, 300);


	imshow("1", d);

	imshow("1_1", a);
	imshow("2", b);
	imshow("3", c);
	bitwise_and(d, a, aa);
	bitwise_and(d, b, bb);
	bitwise_and(d, c, cc);

	imshow("a", aa);
	imshow("b", bb);
	imshow("c", cc);

	cout << "A: " << countNonZero(aa) << "\n";
	cout << countNonZero(bb) << "\n";
	cout << countNonZero(cc) << "\n";



	waitKey(0);

	return 0;

}