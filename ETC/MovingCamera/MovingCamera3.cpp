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
	src_base = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation1.jpg", 1);
	Mat src_base1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation1_2.jpg", 1);
	src_test1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation2_2.jpg", 1);
	src_test2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Rotation3_2.jpg", 1);
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


	//Dynamic Allocate Mat[][]
	//for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
	//
	//	for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){

	//		printf("%d and %d\n", i, j);

	//		if ((src_base.rows - j) < SEG_SIZE && (src_base.cols - i) < SEG_SIZE){
	//			temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
	//			temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
	//			temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));

	//		}
	//		else if ((src_base.rows - j) < SEG_SIZE){
	//			temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j+(src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
	//			temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
	//			temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
	//		}

	//		else if ((src_base.cols - i) < SEG_SIZE){
	//			temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE-1)));
	//			temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE-1)));
	//			temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
	//		}
	//		else{
	//			temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
	//			temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
	//			temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
	//		}

	//	}

	//}



	src_base1.copyTo(a, Mask1);
	src_test1.copyTo(b, Mask2);
	src_test2.copyTo(c, Mask3);


	cvtColor(a, a, CV_BGR2GRAY);
	cvtColor(b, b, CV_BGR2GRAY);
	cvtColor(c, c, CV_BGR2GRAY);
	Canny(a, a, 200, 300);
	Canny(b, b, 200, 300);
	Canny(c, c, 200, 300);
	src_base.copyTo(d, Mask1);
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


	//cvtColor(src_test2, hsv_test2, COLOR_BGR2HSV);

	//hsv_half_down = hsv_base(Range(hsv_base.rows / 2, hsv_base.rows - 1), Range(0, hsv_base.cols - 1));

	///// Using 50 bins for hue and 60 for saturation
	int h_bins = 80; 
	int s_bins = 80; 
	int v_bins = 80;
	
	int histSize[] = { h_bins, s_bins, v_bins};
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

	//for (int i = 0; i < Mat_col; i++){

	//	for (int j = 0; j < Mat_row; j++){

	//		printf("Loop %d %d\n", i, j);
	//		hsv_base = temporary[i][j].clone();
	//		hsv_test1 = temporary2[i][j].clone();
	//		hsv_test2 = temporary3[i][j].clone();


	//		/// Calculate the histograms for the HSV images
	//		calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges);
	//		normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());


	//		calcHist(&hsv_test1, 1, channels, Mat(), hist_test1, 2, histSize, ranges);
	//		normalize(hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat());

	//		calcHist(&hsv_test2, 1, channels, Mat(), hist_test2, 2, histSize, ranges);
	//		normalize(hist_test2, hist_test2, 0, 1, NORM_MINMAX, -1, Mat());

	//		/// Apply the histogram comparison methods
	//		double base_test1 = compareHist(hist_base, hist_test1, 0);
	//		double base_test2 = compareHist(hist_base, hist_test2, 0);

	//		if (base_test1 > 0.5){
	//			match1++;
	//			printf("Base-Test(1), Base-Test(2) : %f, %f \n", base_test1, base_test2);

	//			//imshow("T1", temporary[i][j]);
	//			//imshow("T2", temporary2[i][j]);
	//			//imshow("T3", temporary3[i][j]);
	//			//waitKey(0);

	//			//system("PAUSE");
	//	}
	//		if (base_test2 > 0.5)
	//			match2++;


	//		printf("Base-Test(1), Base-Test(2) : %f, %f \n", base_test1, base_test2);
	//		cout << "==================================================\n" << endl;
	//	}
	//
	//}
	//printf("=============The Match portion==================\n TEST1 = %d / Test2 = %d\n", match1, match2);

//Case of Normal histogram Calculation
	calcHist(&hsv_base, 1, channels, Mask1, hist_base, 2, histSize, ranges);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_base1, 1, channels, Mask1, hist_base1, 2, histSize, ranges);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_test1, 1, channels, Mask2, hist_test1, 2, histSize, ranges);
	normalize(hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_test2, 1, channels, Mask3, hist_test2, 2, histSize, ranges);
	normalize(hist_test2, hist_test2, 0, 1, NORM_MINMAX, -1, Mat());

	/// Apply the histogram comparison methods
	double base_test = compareHist(hist_base, hist_base1, 0);
	double base_test1 = compareHist(hist_base, hist_test1, 0);
	double base_test2 = compareHist(hist_base, hist_test2, 0);



	printf("(1) Base-Test(0), Base-Test(1), Base-Test(2) : %f, %f, %f \n", base_test, base_test1, base_test2);
	cout << "==================================================\n" << endl;

	calcHist(&hsv_base, 1, channels, Mask2, hist_base, 2, histSize, ranges);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	 base_test = compareHist(hist_base, hist_base1, 0);
	 base_test1 = compareHist(hist_base, hist_test1, 0);
	 base_test2 = compareHist(hist_base, hist_test2, 0);

	printf("(2) Base-Test(0), Base-Test(1), Base-Test(2) : %f, %f, %f \n", base_test, base_test1, base_test2);
	cout << "==================================================\n" << endl;

	calcHist(&hsv_base, 1, channels, Mask3, hist_base, 2, histSize, ranges);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	 base_test = compareHist(hist_base, hist_base1, 0);
	 base_test1 = compareHist(hist_base, hist_test1, 0);
	 base_test2 = compareHist(hist_base, hist_test2, 0);

	printf("(3) Base-Test(0), Base-Test(1), Base-Test(2) : %f, %f, %f \n", base_test, base_test1, base_test2);
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