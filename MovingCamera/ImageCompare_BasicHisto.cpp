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
	src_base = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation1_2.jpg", 1);
	Mat src_base1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation2_2.jpg", 1);
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

	//imshow("M", Mask1);
	//imshow("M2", Mask2);
	//imshow("M3", Mask3);

	imshow("Base", src_base);
	imshow("A", src_base1);
	imshow("B", src_test1);
	imshow("C", src_test2);

	int Mat_row = src_base.rows / SEG_SIZE;
	int Mat_col = src_base.cols / SEG_SIZE;

	Mat **temporary = new Mat*[Mat_col];

	for (int z = 0; z < Mat_col; z++){
		temporary[z] = new Mat[Mat_row];
		//printf("%d", z);
	}

	Mat **temporary2 = new Mat*[Mat_col];

	for (int z = 0; z < Mat_col; z++){
		temporary2[z] = new Mat[Mat_row];
		//printf("%d", z);
	}

	Mat **temporary3 = new Mat*[Mat_col];

	for (int z = 0; z < Mat_col; z++){
		temporary3[z] = new Mat[Mat_row];
		//printf("%d", z);
	}


	printf("%d and %d\n", Mat_col, Mat_row);

	cvtColor(src_base, hsv_base, CV_BGR2HSV);
	cvtColor(src_base1, hsv_base1, CV_BGR2HSV);
	cvtColor(src_test1, hsv_test1, CV_BGR2HSV);
	cvtColor(src_test2, hsv_test2, CV_BGR2HSV);

	///// Using 50 bins for hue and 60 for saturation
	int h_bins = 80;
	int s_bins = 80;
	int v_bins = 80;

	int histSize[] = { h_bins, s_bins, v_bins };
	//int histSize[] = { h_bins, s_bins};
	//int histSize[] = { h_bins};

	//// hue varies from 0 to 179, saturation from 0 to 255
	float h_ranges[] = { 0, 256 };
	float s_ranges[] = { 0, 256 };
	float v_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges, v_ranges };
	//const float* ranges[] = { h_ranges, s_ranges};
	//const float* ranges[] = { h_ranges};

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1, 2 };
	//int channels[] = { 0, 1};


	/// Histograms
	MatND hist_base;
	MatND hist_base1;
	MatND hist_test1;
	MatND hist_test2;


	//Case of Normal histogram Calculation
	calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_base1, 1, channels, Mat(), hist_base1, 2, histSize, ranges);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_test1, 1, channels, Mat(), hist_test1, 2, histSize, ranges);
	normalize(hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_test2, 1, channels, Mat(), hist_test2, 2, histSize, ranges);
	normalize(hist_test2, hist_test2, 0, 1, NORM_MINMAX, -1, Mat());

	/// Apply the histogram comparison methods
	double base_test = compareHist(hist_base, hist_base1, 0);
	double base_test1 = compareHist(hist_base, hist_test1, 0);
	double base_test2 = compareHist(hist_base, hist_test2, 0);



	printf("Base-Test(0), Base-Test(1), Base-Test(2) : %f, %f, %f \n", base_test, base_test1, base_test2);
	cout << "==================================================\n" << endl;

	printf("Done \n");


	waitKey(0);

	for (int z = 0; z < Mat_col; ++z){
		delete[] temporary[z];
		delete[] temporary2[z];
		delete[] temporary3[z];
	}

	return 0;

}