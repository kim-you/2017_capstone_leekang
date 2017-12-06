/*----------------------
Winter Vacation Proeject
-----------------------*/

#include "cv.hpp"
#include "opencv2/opencv.hpp"
#include <sstream>
#include <windows.h>
#include <Tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>

using namespace cv;
using namespace std;

int edge_hist_num = 0;

bool calcEdgeDirection3(Mat Origin, int NumBins, float _ratio = 0.8, int threshold=10000);

Mat FindLargestArea(Mat origin, Mat cannies){
	Mat src;
	Mat source;
	int i = 0, count = 0;
	int x = 0, y = 0;
	int nBlue = 0, nGreen = 0, nRed = 0;

	double maxcontour = 0;
	/*
	for (i = 0; i < 3; i++)
	bgr[i] = 0;*/

	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;


	src = origin.clone();

	findContours(cannies, contours, hierarchy, 2, CV_CHAIN_APPROX_SIMPLE);

	for (i = 0; i < contours.size(); i++){
		//		printf("%d = %lf\n", i, contourArea(contours[i]));

		if (contourArea(contours[i])>maxcontour){
			maxcontour = contourArea(contours[i]);
			count = i;
		}

	}
	source = origin.clone();
	cvtColor(source, source, CV_RGB2GRAY);
	source = Scalar(0);
	drawContours(source, contours, count, Scalar(255), CV_FILLED, 8, hierarchy);
	/*
	for (x = 0; x<origin.rows; x++){
	for (y = 0; y<origin.cols; y++){
	if (origin.at<cv::Vec3b>(y, x)[0] == 0 && origin.at<cv::Vec3b>(y, x)[1] == 0 && origin.at<cv::Vec3b>(y, x)[2] == 255){
	nBlue = src.at<cv::Vec3b>(y, x - 1)[0];
	nGreen = src.at<cv::Vec3b>(y, x - 1)[1];
	nRed = src.at<cv::Vec3b>(y, x - 1)[2];
	}
	}
	}
	bgr[0] = nBlue;
	bgr[1] = nGreen;
	bgr[2] = nRed;
	//
	printf("In Function: Blue = %d, Green = %d, Red = %d\n", bgr[0], bgr[1], bgr[2]);
	*/

	return source;
}

vector<float> nomalize(vector<float> a) {

	float vecsize;

	float k = 0;

	for (int i = 0; i < a.size(); i++) {
		k += a[i] * a[i];
	}

	for (int i = 0; i < a.size(); i++)
		a[i] = a[i] / sqrtf(k);

	return a;

}

Mat nonedge_area(Mat src, float sky_rate, int window_size) {
	/*
	Mat src :  ���� ����(����ó����->2��ȭ�������� ��ȯ�� �����̾�����.
	float sky_rate : �ϴÿ� �ش��ϴ� ���� (ex/ 0.3 : ���� 30%�� �����Ѵ�)
	int window_size : �������� ũ�� : �������� �����ϰ� �˻�.
	*/

	int i, i2 = 0;
	int j, j2 = 0;
	int src_height, src_width;

	src_height = src.rows;
	src_width = src.cols;

	Mat window;
	Mat output(src_height, src_width, src.type(), Scalar(0));

	float m_height = src_height * sky_rate;

	for (i = m_height; i + window_size <= src_height; i = i + window_size/5) {
		if (i + window_size >= src_height)
			i2 = src_height;
		else
			i2 = i + window_size;

		for (j = 0; j + window_size <= src_width; j = j + window_size/5) {
			if (j + window_size >= src_width)
				j2 = src_width;
			else
				j2 = j + window_size;

			window = src(Range(i, i2), Range(j, j2));
			if (sum(window) == Scalar(0)) // should be changed.
				output(Range(i, i2), Range(j, j2)) = Scalar(255);
		}
	}
	return output;

}

Mat roadFilter(const Mat& src, double sigma, Mat mask) {
	/* In Lab Color space, Filtering only L's value with sigma*/

	assert(src.type() == CV_8UC3);

	Mat filter;

	Scalar mean;
	Scalar dev;

	double mean_v[3];
	double dev_v[3];
	double sigma_v[3];

	meanStdDev(src, mean, dev, mask);

	for (int i = 0; i < 3; i++)
		sigma_v[i] = (sigma*dev.val[i]);

	for (int i = 0; i < 3; i++){
		mean_v[i] = mean.val[i];
		dev_v[i] = dev.val[i];
	}
	//mask
	//printf("LAB MEAN %lf %lf %lf\n", mean_v[0], mean_v[1], mean_v[2]);
	//printf("LAB DEV %lf %lf %lf\n", dev_v[0], dev_v[1], dev_v[2]);
	//printf("SIGMA %lf %lf %lf\n", sigma_v[0], sigma_v[1], sigma_v[2]);

	if ((sigma_v[1] + sigma_v[2]) <= 20){
		sigma_v[1] = 7;
		sigma_v[2] = 10;
	}

//	printf("SIGMA_FIX %lf %lf %lf\n", sigma_v[0], sigma_v[1], sigma_v[2]);
	inRange(src, Scalar(mean_v[0] - 70, mean_v[1] - sigma_v[1], mean_v[2] - sigma_v[2]), Scalar(255, mean_v[1] + sigma_v[1], mean_v[2] + sigma_v[2]), filter); //Threshold the image

	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));

	return filter;
}

Mat roadFilter2(const Mat& src, double sigma, Mat mask) {

	/*BGR Color Space Filter with Sigma*/

	assert(src.type() == CV_8UC3);
	Mat filter;

	Scalar mean;
	Scalar dev;

	double mean_v[3];
	double dev_v[3];
	double sigma_v[3];

	meanStdDev(src, mean, dev, mask);

	for (int i = 0; i < 3; i++)
		sigma_v[i] = (sigma*dev.val[i]);

	for (int i = 0; i < 3; i++){
		mean_v[i] = mean.val[i];
		dev_v[i] = dev.val[i];
	}
	//mask
	//printf("BGR MEAN %lf %lf %lf\n", mean_v[0], mean_v[1], mean_v[2]);
	//printf("BGR DEV %lf %lf %lf\n", dev_v[0], dev_v[1], dev_v[2]);
	//printf("SIGMA %lf %lf %lf\n", sigma_v[0], sigma_v[1], sigma_v[2]);

	inRange(src, Scalar(mean_v[0] - sigma_v[0], mean_v[1] - sigma_v[1], mean_v[2] - sigma_v[2]), Scalar(200, 200, 200), filter); //Threshold the image

	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));

	return filter;
}


Mat Normalization(Mat src){

	Mat c_lab, canny;
	int low_l = 0;
	Scalar value;
	vector<Mat> lab_images(3);

	cvtColor(src, c_lab, CV_BGR2Lab);
	split(c_lab, lab_images);

	equalizeHist(lab_images[0], lab_images[0]);

	//printf("Light = %d\n", int(value.val[0]));
	merge(lab_images, c_lab);
	cvtColor(c_lab, c_lab, CV_Lab2BGR);



	return c_lab;
}

void callBackFunc2(int event, int x, int y, int flags, void* userdata){
	static int counts_number = 1;
	/* When Mouse Click, Get Scalar Values on Clicked Point */

	Mat src = *(Mat*)userdata;
	Mat src2, src3, src4, src5, src6;

	cvtColor(src, src2, CV_BGR2Lab);
	cvtColor(src, src3, CV_BGR2HSV);
	cvtColor(src, src4, CV_BGR2YCrCb);
	cvtColor(src, src5, CV_BGR2XYZ);

	switch (event){

	case EVENT_LBUTTONDOWN:

		printf("%d : X = %d, Y = %d\n", counts_number, x, y);
		printf("BGR Screen: Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n",
			src.at<Vec3b>(y, x)[0], src.at<Vec3b>(y, x)[1], src.at<Vec3b>(y, x)[2]);
		printf("Lab Screen: Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n",
			src2.at<Vec3b>(y, x)[0], src2.at<Vec3b>(y, x)[1], src2.at<Vec3b>(y, x)[2]);
		printf("HSV Screen: Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n",
			src3.at<Vec3b>(y, x)[0], src3.at<Vec3b>(y, x)[1], src3.at<Vec3b>(y, x)[2]);
		printf("YCrCb Screen: Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n",
			src4.at<Vec3b>(y, x)[0], src4.at<Vec3b>(y, x)[1], src4.at<Vec3b>(y, x)[2]);
		printf("XYZ Screen: Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n",
			src5.at<Vec3b>(y, x)[0], src5.at<Vec3b>(y, x)[1], src5.at<Vec3b>(y, x)[2]);
		printf("-----------------------------------------------------------------\n");

		rectangle(src, Point(x - 5, y - 5), Point(x + 5, y + 5), Scalar(0, 0, 255), 1, 8);

		char str[200];
		sprintf_s(str, "%d", counts_number);
		putText(src, str, Point(x - 7, y - 10), 1, 1, Scalar(0, 0, 255));

		imshow("ORIGIN", src);

		counts_number++;

		break;
	}

}

Mat FindRoad(Mat src){

	Mat back, canny, gray;
	Mat box, box3, lab_back, filter, box4, Color_Mask;

	//back = src.clone();

	bilateralFilter(src, back, 30, 60, 10);

	cvtColor(back, gray, CV_RGB2GRAY);
	//GaussianBlur(gray, gray, Size(7, 7), 0, 0);
	Canny(gray, canny, 15, 25, 3);
	//imshow("CANNY", canny);
	box = nonedge_area(canny, 0.3, 10);
	//imshow("BOX", box);
	box3 = FindLargestArea(src, box); // this is the mask

	//Input Lab Matrix && Largest Area's Mask.

	cvtColor(back, lab_back, CV_BGR2Lab);
	Scalar value = mean(lab_back, box3); // box3 = Mask,
	filter = roadFilter(lab_back, 1.2, box3);
	filter = filter > 128;

	//Input BGR Matrix && Largest Area's Mask.
	box4 = roadFilter2(back, 2.5, box3);
	box4 = box4 > 128;

	//AND MASK FILTER&&BOX4
	bitwise_and(filter, box4, Color_Mask);

	return Color_Mask;
}

bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r)
{

	Point2f x = o2 - o1;
	Point2f d1 = p1 - o1;
	Point2f d2 = p2 - o2;

	float cross = d1.x*d2.y - d1.y*d2.x;
	if (abs(cross) < /*EPS*/1e-8)
		return false;

	double t1 = (x.x * d2.y - x.y * d2.x) / cross;
	r = o1 + d1 * t1;
	return true;
}

double dist(Point2f A, Point2f B) {
	double X_diff = 0, Y_diff = 0;
	X_diff = (A.x - B.x) * (A.x - B.x);
	Y_diff = (A.y - B.y) * (A.y - B.y);
	return sqrt(X_diff + Y_diff);
}

int ifLRline(Point2f A, Point2f B, Point2f P) {
	if (A.x != B.x) {
		double gradiant = (B.y - A.y) / (B.x - A.x);
		double D = gradiant*(P.x - A.x) + A.y;
		if (P.y > D)
			return 1;
		else if (P.y < D)
			return -1;
		else
			return 0;
	}
}

void OpticalFlow_Count(int Pnum, vector<uchar> status, int & Car_num, Mat& frame, Point2f & pass, vector<Point2f> after, vector<Point2f> Center, Point2f A, Point2f B) {
	//		calcOpticalFlowPyrLK(former_gray, latter_gray, Center, after, status, err, Size(25, 25), 3);
	for (int i = 0; i < Pnum; i++) {
		if (status[i] == 0) // if the center[i] doesn't exist at the former frame
			continue;		// continue
		else {
			circle(frame, after[i], 3, Scalar(0, 0, 255), 3, 8);	// draw estimated point
			Point2f P;
			if (intersection(Center[i], after[i], A, B, P)) {	// C - a __P__ A - B
				if ((((A.x <= P.x) && (P.x <= B.x)) || ((B.x <= P.x) && (P.x <= A.x))) && (((A.y <= P.y) && (P.y <= B.y)) || ((B.y <= P.y) && (P.y <= A.y)))) {
					//o1,p1  -- o2, p2
					if (ifLRline(A, B, after[i]) * ifLRline(A, B, Center[i]) < 0) {	//	if Center[i] is on line A-B
						if (dist(P, pass) < 4) { //	avoid double counting
							pass = Point(0, 0);	 // only once
							continue;
						}
						pass = P;	//save the point which is on the line
						Car_num++;
						line(frame, A, B, Scalar(0, 255, 255), 10, 10, 0);
					}
				}
			}
		}
	}

}



void detect_haarcascades(Mat src, string path="")
{
	static boolean flag = 0;

	Mat frame, frame2;
	Mat fore, gray;

	frame = src.clone();

	if (frame.empty()) return;


	int w = frame.size().width;
	int h = frame.size().height;


	//frame size


	__int64 freq, start, finish;
	::QueryPerformanceFrequency((_LARGE_INTEGER*)&freq);

	uchar *temp_ptr;
	uchar *temp_ptr2;


	/* code block 1 */
	//4000 Negative Test
	//string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\MachineLearning\\cascade_Test\\cascade.xml";
	//LBP
	string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\MachineLearning\\cascade2\\cascade.xml";
	//HOG
	//string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\MachineLearning\\cascade_Haar\\cascade.xml";
	//SVM
	//string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\MachineLearning\\trainedSVM.xml";



	static CascadeClassifier detector;

	if (flag == 0){

		if (path.empty()) printf("Path Empty\n");

		if (!(path.empty())){
			printf("Not Empty\n");
			string cascadeName = path;
		}

		printf("The path is %s\n", cascadeName.c_str());

		if (!detector.load(cascadeName))
		{
			printf("ERROR: Could not load classifier cascade\n");
			exit(0);
		}
	}

	//casecade function parameters
	int gr_thr = 3;
	double scale_step = 1.1;


	//object size
	Size min_obj_sz_step(25, 25);
	Size max_obj_sz_step(180, 180);



		frame = src.clone();
		// input image
		if (frame.empty()) return;

		//processing time set
		::QueryPerformanceCounter((_LARGE_INTEGER*)&start);

		Mat ROI;
		Rect ROI_rect(w*0.1, h*0.1, w*0.8, h*0.8);
		//ROI = frame(ROI_rect);
		ROI = frame.clone();


		/* code block 2 */
		vector<Rect> found;
		detector.detectMultiScale(ROI, found, scale_step, gr_thr, 0, min_obj_sz_step, max_obj_sz_step);


		////add offset
		//for (int i = 0; i < (int)found.size(); i++) {
		//	found[i].x += w*0.1;
		//	found[i].y += h*0.1;
		//}

		//draw rectangles
		for (int i = 0; i < (int)found.size(); i++)
		{
			rectangle(frame, found[i], Scalar(0, 255, 0), 3);
		}

		// processing time check (fps)
		::QueryPerformanceCounter((_LARGE_INTEGER*)&finish);
		double fps = freq / double(finish - start + 1);
		char fps_str[20];
		sprintf_s(fps_str, 20, "FPS: %.1lf", fps);
		putText(frame, fps_str, Point(5, 35), FONT_HERSHEY_SIMPLEX, 1., Scalar(0, 0, 255), 2);


		//show image
		imshow("raw image", frame);
		//imshow("ROI image", ROI);

		//hit 'space' -> step
		//hit 'ESC' -> escape
		//char ch = waitKey(10);
		//if (ch == 27) break;            // ESC Key
		//else if (ch == 32)               // SPACE Key
		//{
		//	while ((ch = waitKey(10)) != 32 && ch != 27);
		//	if (ch == 27) break;
		//}


		flag = 1;
}

int DistHisto(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE){
	int temp_thres = 0;

	Mat mask1 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\TrafficExample\\Mask1.jpg");
	Mat mask2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\TrafficExample\\Mask2.jpg");
	Mat mask3 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\TrafficExample\\Mask3.jpg");

	resize(mask1, mask1, Size(Origin.cols, Origin.rows));
	resize(mask2, mask2, Size(Origin.cols, Origin.rows));
	resize(mask3, mask3, Size(Origin.cols, Origin.rows));
	mask1 = mask1 > 128;
	mask2 = mask2 > 128;
	mask3 = mask3 > 128;




	if (Origin.empty() || compare1.empty() || compare2.empty() || compare3.empty()){
		cout << "DIST HISTO ERROR | NO FILES" << endl;
		return -1;
	}

	Mat src_base, src_base1, hsv_base, hsv_base1;
	Mat src_test1, hsv_test1;
	Mat src_test2, hsv_test2;
	Mat src_line;
	int match0 = 0;
	int match1 = 0;
	int match2 = 0;

	src_base = Origin.clone();

	compare1.copyTo(src_base1, mask1);
	compare2.copyTo(src_test1, mask2);
	compare3.copyTo(src_test2, mask3);

	//src_base1 = compare1.clone();
	//src_test1 = compare2.clone();
	//src_test2 = compare3.clone();

	cvtColor(src_base, hsv_base, CV_BGR2HSV);
	cvtColor(src_base1, hsv_base1, CV_BGR2HSV);
	cvtColor(src_test1, hsv_test1, CV_BGR2HSV);
	cvtColor(src_test2, hsv_test2, CV_BGR2HSV);

	src_base.copyTo(src_line);

	//Allocate Array Size
	int Mat_row = Origin.rows / SEG_SIZE + 1;
	int Mat_col = Origin.cols / SEG_SIZE + 1;

	if (Origin.rows % SEG_SIZE == 0){
		Mat_row = Origin.rows / SEG_SIZE;
	}

	if (Origin.cols % SEG_SIZE == 0){
		Mat_col = Origin.cols / SEG_SIZE;
	}


	//Dynamic Allocation.
	Mat **temporary = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary[z] = new Mat[Mat_row];

	Mat **temporary2 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary2[z] = new Mat[Mat_row];

	Mat **temporary3 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary3[z] = new Mat[Mat_row];

	Mat **temporary4 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary4[z] = new Mat[Mat_row];

	//printf("Matrix [%d x %d] Allocated \n", Mat_col, Mat_row);

	//Dynamic Allocate Mat[][]
	for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){


		for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){

		//	printf("%d and %d\n", i, j);

			if ((src_base.rows - j) < SEG_SIZE && (src_base.cols - i) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = hsv_base1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));

			}
			else if ((src_base.rows - j) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = hsv_base1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
			}

			else if ((src_base.cols - i) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = hsv_base1(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
			}
			else{
				temporary[i / SEG_SIZE][j / SEG_SIZE] = hsv_base(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = hsv_test1(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = hsv_test2(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = hsv_base1(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
			}

		}

	}



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


	//Draw Line Image for comparision.
	for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
		line(src_line, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
		//line(test_mask, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
	}

	for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){
		line(src_line, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
		//line(test_mask, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
	}

	for (int i = 0; i < Mat_col; i++){

		for (int j = 0; j < Mat_row; j++){

			char str[200];

			//	printf("Loop %d %d\n", i, j);

			hsv_base = temporary[i][j].clone();
			hsv_base1 = temporary4[i][j].clone();
			hsv_test1 = temporary2[i][j].clone();
			hsv_test2 = temporary3[i][j].clone();





			/// Calculate the histograms for the HSV images
			calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges);
			normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

			calcHist(&hsv_base1, 1, channels, Mat(), hist_base1, 2, histSize, ranges);
			normalize(hist_base1, hist_base1, 0, 1, NORM_MINMAX, -1, Mat());

			calcHist(&hsv_test1, 1, channels, Mat(), hist_test1, 2, histSize, ranges);
			normalize(hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat());

			calcHist(&hsv_test2, 1, channels, Mat(), hist_test2, 2, histSize, ranges);
			normalize(hist_test2, hist_test2, 0, 1, NORM_MINMAX, -1, Mat());

			/// Apply the histogram comparison methods
			double max_base = 0;
			double base_test0 = compareHist(hist_base, hist_base1, 0);
			double base_test1 = compareHist(hist_base, hist_test1, 0);
			double base_test2 = compareHist(hist_base, hist_test2, 0);

			max_base = MAX(base_test0, MAX(base_test1, base_test2));



			if (max_base == base_test0 && max_base == base_test1 && max_base == base_test2){
				putText(src_line, "X", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 255, 255), 2, 4, false);
				continue;
			}

			//if (base_test0 > temp_thres || base_test1 > temp_thres || base_test2 > temp_thres){

			//	if (base_test0 > temp_thres)
			//		match0++;
			//	if (base_test1 > temp_thres)
			//		match1++;
			//	if (base_test2 > temp_thres)
			//		match2++;

			////	printf("Check!\n");
			////	printf("Base-Test(0) Base-Test(1), Base-Test(2) : %f, %f, %f \n", base_test0, base_test1, base_test2);
			//	//imshow("T1", temporary[i][j]);
			//	//imshow("T2", temporary2[i][j]);
			//	//imshow("T3", temporary3[i][j]);
			//	//waitKey(0);

			//	//				system("PAUSE");
			//}


		//	cout << "MAXIMUM : " << max_base << endl;

			else if (max_base == base_test0){
				putText(src_line, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				match0++;
			}
			else if (max_base == base_test1){
				putText(src_line, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
				//	putText(test_mask, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
				match1++;
			}
			else if (max_base == base_test2){
				putText(src_line, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
				//	putText(test_mask, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
				match2++;
			}


//			printf("Base-Test(0) Base-Test(1), Base-Test(2) : %f, %f, %f \n", base_test0, base_test1, base_test2);
	//		cout << "==================================================\n" << endl;
		}

	}
	printf("=================Histo portion==================\n Test0 = %d / TEST1 = %d / Test2 = %d\n", match0, match1, match2);
	//imshow("TEST_HIST", hist_base);
	imshow("HISTOLINE", src_line);

	for (int z = 0; z < Mat_col; ++z){
		delete[] temporary[z];
		delete[] temporary2[z];
		delete[] temporary3[z];
		delete[] temporary4[z];
	}

	int max_match = MAX(match0, MAX(match1, match2));

	if (max_match == match0)
		return 1;

	if (max_match == match1)
		return 2;

	if (max_match == match2)
		return 3;


}

int* calcEdgeDirection(Mat Origin, int NumBins){

	Mat src, gray, sobel_x, sobel_y, result;

	src = Origin.clone();

	//Dynamic allocation
	vector<vector<float>> direction(src.cols, vector<float>(src.rows, 0));

	int bin_degree = 360 / NumBins;

	vector<int> bin_hist(NumBins);

	cvtColor(src, gray, CV_BGR2GRAY);

	//GaussianBlur(gray, gray, Size(3, 3), 0, 0);

	//Sobel(gray, sobel_x, CV_8UC1, 1, 0, 3, 1, 0, BORDER_REPLICATE);
	//Sobel(gray, sobel_y, CV_8UC1, 0, 1, 3, 1, 0, BORDER_REPLICATE);
	Sobel(gray, sobel_x, CV_32FC1, 1, 0);
	Sobel(gray, sobel_y, CV_32FC1, 0, 1);

	//sobel_x.convertTo(sobel_x, CV_32FC1);

	Mat Mag(gray.size(), CV_32FC1);
	Mat Angle(gray.size(), CV_32FC1);

	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);

	addWeighted(sobel_x, 0.5, sobel_y, 0.5, 0, result);

	//imshow("RESULT", result);

	for (int i = 0; i < Angle.rows; i++){

		for (int j = 0; j < Angle.cols; j++){
			//cout << sobel_x.at<float>(i,j) << endl;
			//cout << Angle.at<float>(i, j) << endl;

			//if (Mag.at<float>(i, j) >= 10){
				direction[j][i] = Angle.at<float>(i, j);
//			}


			for (int z = 0; z < NumBins; z++){
				if (direction[j][i] > bin_degree*z && direction[j][i] <= bin_degree*(z + 1))
					bin_hist[z]++;
			}

		}
	}



	vector<int> temporary(NumBins);

	temporary = bin_hist;

	sort(temporary.begin(), temporary.end(), greater<>());

	int temp_flag = 0;
	int maximum = 0;
	int maximum2 = 0;

	for (int z = 0; z < NumBins; z++){

	//	printf("NumBin[%d] (%d ~  %d) = %d\n", z, z*bin_degree, (z + 1)*bin_degree, bin_hist[z]);

		if (bin_hist[z] == temporary[0] && temp_flag==0){
			maximum = z;
			temp_flag++;
		}
		else if (bin_hist[z] == temporary[1]){
			maximum2 = z;
		}

	}

//	printf("MAXIMUM = %d, 2_MAXIMUM = %d\n", maximum, maximum2);

	int* t = new int[2];

	t[0] = maximum;
	t[1] = maximum2;


	//Deallocate 2 dimension Vector
	for (int i = 0; i < direction.size(); i++)
		direction[i].clear();

	direction.clear();
	temporary.clear();
	bin_hist.clear();

	return t;

}

int* calcEdgeDirection2(Mat Origin, int NumBins){

	Mat src, gray, sobel_x, sobel_y, result, canny, mask_x, mask_y;

	src = Origin.clone();

	//Dynamic allocation
	vector<vector<float>> direction(src.cols, vector<float>(src.rows, 0));

	int bin_degree = 180 / NumBins;

	vector<float> bin_hist(NumBins, 0);

	cvtColor(src, gray, CV_BGR2GRAY);

	Sobel(gray, sobel_x, CV_32FC1, 1, 0, -1);
	Sobel(gray, sobel_y, CV_32FC1, 0, 1, -1);

	Mat Mag(gray.size(), CV_32FC1);
	Mat Angle(gray.size(), CV_32FC1);

	//Canny(gray, canny, 50, 100);

	//sobel_x.copyTo(mask_x, canny);
	//sobel_y.copyTo(mask_y, canny);
	//
	//imshow("MASK_X", mask_x + mask_y);


	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);


	for (int i = 0; i < Angle.rows; i++){

		for (int j = 0; j < Angle.cols; j++){

			//cout << sobel_x.at<float>(i,j) << endl;
			//cout << Angle.at<float>(i, j) << endl;

			float tempangle = Angle.at<float>(i, j);

			if (tempangle >= 180)
				tempangle -= 180;

			int a = tempangle / bin_degree;

			float k = tempangle - (a*bin_degree);

			float r = k / bin_degree;

			float tempmag = Mag.at<float>(i, j);

//			if (tempmag > 100)
				//printf("Angle = %f, Magnitude = %f, BIN_HIST = %d\n", tempangle, tempmag, a);

			bin_hist[a] += tempmag*(1 - r);

			if ((a + 1) * bin_degree >= 180){
				bin_hist[0] += tempmag*r;
			}
			else{
				bin_hist[a + 1] += tempmag*r;
			}


		}
	}


	vector<float> temporary(NumBins);

	temporary = bin_hist;

	sort(temporary.begin(), temporary.end(), greater<>());

	int temp_flag = 0;
	int maximum = 0;
	int maximum2 = 0;

	float sum = 0;

	vector<float> Normalized(NumBins);

	for (int z = 0; z < NumBins; z++){

		printf("NumBin[%d] (%d ~  %d) = %f\n", z, z*bin_degree, (z + 1)*bin_degree, bin_hist[z]);

		sum += bin_hist[z];

		Normalized = nomalize(bin_hist);

		if (bin_hist[z] == temporary[0] && temp_flag == 0){
			maximum = z;
			temp_flag++;
		}
		else if (bin_hist[z] == temporary[1]){
			maximum2 = z;
		}

	}


	for (int i = 0; i < NumBins; i++){
		printf("Normal[%d] = %lf\n", i, Normalized[i]);
	}

	int* t = new int[2];

	t[0] = maximum;
	t[1] = maximum2;

	printf("SUM = %f\n", sum);

	for (int z = 0; z < NumBins; z++){
		printf("PERCENT[%d] = %lf\n", z, bin_hist[z] / sum * float(100));
	}
	////Deallocate 2 dimension Vector
	//for (int i = 0; i < src.cols; i++)
	//	direction[i].clear();

	//direction.clear();
	//temporary.clear();
	//bin_hist.clear();

	return t;

}



Mat DistEdgeCompare(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE){


	Mat src_base, src_base1;
	Mat src_test1;
	Mat src_test2;

	int match0 = 0;
	int match1 = 0;
	int match2 = 0;

	int match0_1 = 0;
	int match1_1 = 0;
	int match2_1 = 0;

	int test1 = 0;
	int test2 = 0;
	int test3 = 0;
	int test4 = 0;

	src_base = Origin.clone();
	src_base1 = compare1.clone();
	src_test1 = compare2.clone();
	src_test2 = compare3.clone();

	Mat src_line = src_base.clone();

	//Allocate Array Size
	int Mat_row = Origin.rows / SEG_SIZE + 1;
	int Mat_col = Origin.cols / SEG_SIZE + 1;

	if (Origin.rows % SEG_SIZE == 0){
		Mat_row = Origin.rows / SEG_SIZE;
	}

	if (Origin.cols % SEG_SIZE == 0){
		Mat_col = Origin.cols / SEG_SIZE;
	}

	//Dynamic Allocation.
	Mat **temporary = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary[z] = new Mat[Mat_row];

	Mat **temporary2 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary2[z] = new Mat[Mat_row];

	Mat **temporary3 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary3[z] = new Mat[Mat_row];

	Mat **temporary4 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary4[z] = new Mat[Mat_row];

	//printf("Matrix [%d x %d] Allocated \n", Mat_col, Mat_row);

	//Dynamic Allocate Mat[][]
	for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
		for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){

			//printf("%d and %d\n", i, j);

			if ((src_base.rows - j) < SEG_SIZE && (src_base.cols - i) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));

			}
			else if ((src_base.rows - j) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
			}

			else if ((src_base.cols - i) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
			}
			else{
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
			}

		}

	}

	//Check for Calc Edge Direction.
	int* ED_src;
	int* ED_src2;
	int* ED_src3;
	int* ED_src4;

	int max_base = 0;
	int max_base_1 = 0;

	//Draw Line Image for comparision.
	for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
		line(src_line, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
		//line(test_mask, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
	}

	for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){
		line(src_line, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
		//line(test_mask, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
	}

	for (int i = 0; i < Mat_col; i++){

		for (int j = 0; j < Mat_row; j++){

			char str[200];
		//	printf("Loop %d %d\n", i, j);
			ED_src = calcEdgeDirection(temporary[i][j], 9);
			ED_src2 = calcEdgeDirection(temporary4[i][j], 9);
			ED_src3 = calcEdgeDirection(temporary2[i][j], 9);
			ED_src4 = calcEdgeDirection(temporary3[i][j], 9);

			for (int z = 0; z < 2; z++){
		//		printf("CHECK[%d] %d %d %d %d\n", z, ED_src[z], ED_src2[z], ED_src3[z], ED_src4[z]);
			}

			if (ED_src[0] == ED_src2[0])
				match0++;
			if (ED_src[0] == ED_src3[0])
				match1++;
			if (ED_src[0] == ED_src4[0])
				match2++;
			if (ED_src[1] == ED_src2[1])
				match0_1++;
			if (ED_src[1] == ED_src3[1])
				match1_1++;
			if (ED_src[1] == ED_src4[1])
				match2_1++;


			if (match0 == match1 || match1 == match2 || match0 == match2){

				if (ED_src[1] == ED_src2[1] && ED_src[0] == ED_src2[0]){
					putText(src_line, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					//printf("1 is the best match\n");
					test1++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[1] == ED_src3[1] && ED_src[0] == ED_src3[0]){
					putText(src_line, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
					//printf("2 is the best match\n");
					test2++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[1] == ED_src4[1] && ED_src[0] == ED_src4[0]){
					putText(src_line, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
				//	printf("3 is the best match\n");
					test3++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else{
					if (ED_src[0] == ED_src2[0]){
						putText(src_line, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
						//printf("1 is the best match\n");
						test1++;
						//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					}
					else if (ED_src[0] == ED_src3[0]){
						putText(src_line, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
						//printf("2 is the best match\n");
						test2++;
						//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					}
					else if (ED_src[0] == ED_src4[0]){
						putText(src_line, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
						//printf("3 is the best match\n");
						test3++;
						//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					}
					else{
						putText(src_line, "X", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 255, 255), 2, 4, false);
					//	printf("NO MATCH\n");
						test4++;
					}
				}

			}
			else{
				if (ED_src[0] == ED_src2[0]){
					putText(src_line, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				//	printf("1 is the best match\n");
					test1++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[0] == ED_src3[0]){
					putText(src_line, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
				//	printf("2 is the best match\n");
					test2++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[0] == ED_src4[0]){
					putText(src_line, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
			//		printf("3 is the best match\n");
					test3++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else{
					putText(src_line, "X", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 255, 255), 2, 4, false);
				//	printf("NO MATCH\n");
					test4++;
				}
			}

			//printf("===============================================================\n");
			match0 = 0;
			match1 = 0;
			match2 = 0;
			match0_1 = 0;
			match1_1 = 0;
			match2_1 = 0;

		}

	}

	imshow("EDGELine", src_line);

	printf("=================Edge Portion===================\n");
	printf("(1) %d, (2) %d, (3) %d, (N) %d\n", test1, test2, test3, test4);

	for (int z = 0; z < Mat_col; ++z){
		delete[] temporary[z];
		delete[] temporary2[z];
		delete[] temporary3[z];
		delete[] temporary4[z];
	}

	int max_match = MAX(match0, MAX(match1, match2));

	if (max_match == match0)
		return compare1;

	if (max_match == match1)
		return compare2;

	if (max_match == match2)
		return compare3;

}


Mat DistEdgeCompare2(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE){


	Mat src_base, src_base1;
	Mat src_test1;
	Mat src_test2;

	int match0 = 0;
	int match1 = 0;
	int match2 = 0;

	int match0_1 = 0;
	int match1_1 = 0;
	int match2_1 = 0;

	int test1 = 0;
	int test2 = 0;
	int test3 = 0;
	int test4 = 0;

	src_base = Origin.clone();
	src_base1 = compare1.clone();
	src_test1 = compare2.clone();
	src_test2 = compare3.clone();

	Mat src_line = src_base.clone();

	//Allocate Array Size
	int Mat_row = Origin.rows / SEG_SIZE + 1;
	int Mat_col = Origin.cols / SEG_SIZE + 1;

	if (Origin.rows % SEG_SIZE == 0){
		Mat_row = Origin.rows / SEG_SIZE;
	}

	if (Origin.cols % SEG_SIZE == 0){
		Mat_col = Origin.cols / SEG_SIZE;
	}

	//Dynamic Allocation.
	Mat **temporary = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary[z] = new Mat[Mat_row];

	Mat **temporary2 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary2[z] = new Mat[Mat_row];

	Mat **temporary3 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary3[z] = new Mat[Mat_row];

	Mat **temporary4 = new Mat*[Mat_col];
	for (int z = 0; z < Mat_col; z++)
		temporary4[z] = new Mat[Mat_row];

	//printf("Matrix [%d x %d] Allocated \n", Mat_col, Mat_row);

	//Dynamic Allocate Mat[][]
	for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
		for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){

			//printf("%d and %d\n", i, j);

			if ((src_base.rows - j) < SEG_SIZE && (src_base.cols - i) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));

			}
			else if ((src_base.rows - j) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
			}

			else if ((src_base.cols - i) < SEG_SIZE){
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
			}
			else{
				temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary2[i / SEG_SIZE][j / SEG_SIZE] = src_test1(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary3[i / SEG_SIZE][j / SEG_SIZE] = src_test2(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				temporary4[i / SEG_SIZE][j / SEG_SIZE] = src_base1(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
			}

		}

	}

	//Check for Calc Edge Direction.
	int* ED_src;
	int* ED_src2;
	int* ED_src3;
	int* ED_src4;

	int max_base = 0;
	int max_base_1 = 0;

	//Draw Line Image for comparision.
	for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
		line(src_line, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
		//line(test_mask, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
	}

	for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){
		line(src_line, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
		//line(test_mask, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
	}

	for (int i = 0; i < Mat_col; i++){

		for (int j = 0; j < Mat_row; j++){

			char str[200];
			//	printf("Loop %d %d\n", i, j);
			ED_src = calcEdgeDirection2(temporary[i][j], 9);
			ED_src2 = calcEdgeDirection2(temporary4[i][j], 9);
			ED_src3 = calcEdgeDirection2(temporary2[i][j], 9);
			ED_src4 = calcEdgeDirection2(temporary3[i][j], 9);

			for (int z = 0; z < 2; z++){
				//		printf("CHECK[%d] %d %d %d %d\n", z, ED_src[z], ED_src2[z], ED_src3[z], ED_src4[z]);
			}

			if (ED_src[0] == ED_src2[0])
				match0++;
			if (ED_src[0] == ED_src3[0])
				match1++;
			if (ED_src[0] == ED_src4[0])
				match2++;
			if (ED_src[1] == ED_src2[1])
				match0_1++;
			if (ED_src[1] == ED_src3[1])
				match1_1++;
			if (ED_src[1] == ED_src4[1])
				match2_1++;


			if (match0 == match1 || match1 == match2 || match0 == match2){

				if (ED_src[1] == ED_src2[1] && ED_src[0] == ED_src2[0]){
					putText(src_line, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					//printf("1 is the best match\n");
					test1++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[1] == ED_src3[1] && ED_src[0] == ED_src3[0]){
					putText(src_line, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
					//printf("2 is the best match\n");
					test2++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[1] == ED_src4[1] && ED_src[0] == ED_src4[0]){
					putText(src_line, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
					//	printf("3 is the best match\n");
					test3++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else{
					if (ED_src[0] == ED_src2[0]){
						putText(src_line, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
						//printf("1 is the best match\n");
						test1++;
						//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					}
					else if (ED_src[0] == ED_src3[0]){
						putText(src_line, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
						//printf("2 is the best match\n");
						test2++;
						//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					}
					else if (ED_src[0] == ED_src4[0]){
						putText(src_line, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
						//printf("3 is the best match\n");
						test3++;
						//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					}
					else{
						putText(src_line, "X", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 255, 255), 2, 4, false);
						//	printf("NO MATCH\n");
						test4++;
					}
				}

			}
			else{
				if (ED_src[0] == ED_src2[0]){
					putText(src_line, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
					//	printf("1 is the best match\n");
					test1++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[0] == ED_src3[0]){
					putText(src_line, "2", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 2, 4, false);
					//	printf("2 is the best match\n");
					test2++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else if (ED_src[0] == ED_src4[0]){
					putText(src_line, "3", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 2, 4, false);
					//		printf("3 is the best match\n");
					test3++;
					//putText(test_mask, "1", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 2, 4, false);
				}
				else{
					putText(src_line, "X", Point(i*SEG_SIZE + SEG_SIZE / 2, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 255, 255), 2, 4, false);
					//	printf("NO MATCH\n");
					test4++;
				}
			}

			//printf("===============================================================\n");
			match0 = 0;
			match1 = 0;
			match2 = 0;
			match0_1 = 0;
			match1_1 = 0;
			match2_1 = 0;

		}

	}

	imshow("EdgeLine2", src_line);


	printf("=================Edge Portion===================\n");
	printf("(1) %d, (2) %d, (3) %d, (N) %d\n", test1, test2, test3, test4);

	for (int z = 0; z < Mat_col; ++z){
		delete[] temporary[z];
		delete[] temporary2[z];
		delete[] temporary3[z];
		delete[] temporary4[z];
	}

	int max_match = MAX(match0, MAX(match1, match2));

	if (max_match == match0)
		return compare1;

	if (max_match == match1)
		return compare2;

	if (max_match == match2)
		return compare3;

}


void splitEdgeDirection(Mat Origin, int BIN_SIZE, int SEG_SIZE=0){

	Mat gray, sobel_x, sobel_y, result;
	Mat src, canny, mask;

	Mat src_base = Origin.clone();

	bilateralFilter(Origin, src, 25, 30, 50);
	cvtColor(src, gray, CV_BGR2GRAY);

	Sobel(gray, sobel_x, CV_32F, 1, 0, 1);
	Sobel(gray, sobel_y, CV_32F, 0, 1, 1);

	//sobel_x.convertTo(sobel_x, CV_32FC1);
	//imshow("asdfasdf", sobel_x);

	Canny(gray, canny, 10, 20);
	addWeighted(sobel_x, 0.5, sobel_y, 0.5, 0, result);

	result.copyTo(mask, canny);


	//imshow("RESULT2", mask);

	if (SEG_SIZE == 0){
		char str[200];

		Mat src_line = Origin.clone();

		int* check_direct;

		check_direct = calcEdgeDirection2(Origin, BIN_SIZE);

		edge_hist_num = check_direct[0];

		printf("==========================================\n");
		//imshow("SPLIT_SRC", src);
		imshow("SSPLIT_LINE", src_line);

	}
	else{

		//Allocate Array Size
		int Mat_row = src.rows / SEG_SIZE + 1;
		int Mat_col = src.cols / SEG_SIZE + 1;

		if (src.rows % SEG_SIZE == 0){
			Mat_row = src.rows / SEG_SIZE;
		}

		if (src.cols % SEG_SIZE == 0){
			Mat_col = src.cols / SEG_SIZE;
		}

		//Dynamic Allocation.
		Mat **temporary = new Mat*[Mat_col];


		for (int z = 0; z < Mat_col; z++)
			temporary[z] = new Mat[Mat_row];

		//Dynamic Allocate Mat[][]
		for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
			for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){

				//printf("%d and %d\n", i, j);

				if ((src_base.rows - j) < SEG_SIZE && (src_base.cols - i) < SEG_SIZE){
					temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1)));

				}
				else if ((src_base.rows - j) < SEG_SIZE){
					temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + SEG_SIZE));
				}

				else if ((src_base.cols - i) < SEG_SIZE){
					temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + SEG_SIZE), Range(i, i + (src_base.cols%SEG_SIZE - 1)));
				}
				else{
					temporary[i / SEG_SIZE][j / SEG_SIZE] = src_base(Range(j, j + SEG_SIZE), Range(i, i + SEG_SIZE));
				}

			}

		}

		Mat src_line = src_base.clone();

		//Draw Line Image for comparision.
		for (int i = 0; i < src_base.cols; i = i + SEG_SIZE){
			line(src_line, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
			//line(test_mask, Point(i, 0), Point(i, src_base.rows), Scalar(0, 255, 255), 1, 4);
		}

		for (int j = 0; j < src_base.rows; j = j + SEG_SIZE){
			line(src_line, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
			//line(test_mask, Point(0, j), Point(src_base.cols, j), Scalar(0, 255, 255), 1, 4);
		}

		int* check_direct;

		for (int i = 0; i < Mat_col; i++){

			for (int j = 0; j < Mat_row; j++){

				char str[200];
				printf("[%d][%d]\n", i, j);
				check_direct = calcEdgeDirection2(temporary[i][j], BIN_SIZE);

				for (int z = 0; z < BIN_SIZE; z++){
					sprintf_s(str, "%d", z);
					float size = SEG_SIZE*0.3;
					float size2 = SEG_SIZE*0.4;

					if (check_direct[0] == z){
						switch (z){
						case -1:
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 0), 1, 2, false);
							break;
						case 0:
							//		rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(255, 0, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 0), 1, 2, false);
							break;
						case 1:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(100, 0, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 0), 1, 2, false);
							break;
						case 2:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(0, 0, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 255), 1, 4, false);
							break;
						case 3:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(0, 255, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 255, 0), 1, 4, false);
							break;
						case 4:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(0, 100, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(255, 0, 255), 1, 4, false);
							break;
						case 5:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(0, 0, 255), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 255, 255), 1, 4, false);
							break;
						case 6:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(0, 0, 100), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(120, 120, 120), 1, 4, false);
							break;
						case 7:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(255, 255, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(120, 0, 0), 1, 4, false);
							break;
						case 8:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(100, 255, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 120, 0), 1, 4, false);
							break;
						case 9:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(100, 100, 0), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 0, 120), 1, 4, false);
							break;
						case 10:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(255, 0, 255), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(0, 120, 120), 1, 4, false);
							break;
						case 11:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(255, 0, 100), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(120, 0, 120), 1, 4, false);
							break;
						case 12:
							//rectangle(src_line, Rect(i*SEG_SIZE + size, j*SEG_SIZE + size, size2, size2), Scalar(100, 0, 100), -1);
							putText(src_line, str, Point(i*SEG_SIZE + SEG_SIZE / 10, j*SEG_SIZE + SEG_SIZE / 2), 1, 1, Scalar(120, 120, 0), 1, 4, false);
							break;
						}
					}
				}

			}
		}

		for (int z = 0; z < Mat_col; ++z){
			delete[] temporary[z];
		}

		//imshow("SPLIT_SRC", src);
		imshow("SPLIT_LINE", src_line);
	}
}



void callBackFunc3(int event, int x, int y, int flags, void* userdata){

	static int counts_number = 1;
	int SEG_SIZE = 15; //���� ũ�� �����ߵ�.

	/* When Mouse Click, Get Scalar Values on Clicked Point */

	Mat src = *(Mat*)userdata;
	Mat src2 = src.clone();
	Mat src_slice;





	/*
	src_base(Range(j, j + (src_base.rows%SEG_SIZE)), Range(i, i + (src_base.cols%SEG_SIZE - 1))
	*/

	switch (event){

	case EVENT_LBUTTONDOWN:

		printf("[%d]\n", counts_number);
		src_slice = src(Range(y-SEG_SIZE/2, y + SEG_SIZE/2), Range(x-SEG_SIZE/2, x + SEG_SIZE/2));

		splitEdgeDirection(src_slice, 9);

		rectangle(src2, Point(x - SEG_SIZE/2, y - SEG_SIZE/2), Point(x + SEG_SIZE/2, y + SEG_SIZE/2), Scalar(0, 0, 255), 1, 8);

		char str[200];
		char str2[200];
		sprintf_s(str, "%d", counts_number);
		sprintf_s(str2, "%d", edge_hist_num);

		putText(src2, str, Point(x - SEG_SIZE/2, y - SEG_SIZE), 1, 1, Scalar(0, 0, 255));
		putText(src2, str2, Point(x, y), 1, 1, Scalar(255, 255, 0));

		imshow("CHECK_DIRECTION", src2);

		counts_number++;

		break;
	}

	edge_hist_num = 0;

}

Mat direct_area(Mat src, int window_size, float ratio, float sky_rate = 0) {
	/*
	Mat src :  ���� ����(����ó����->2��ȭ�������� ��ȯ�� �����̾�����.
	float sky_rate : �ϴÿ� �ش��ϴ� ���� (ex/ 0.3 : ���� 30%�� �����Ѵ�)
	int window_size : �������� ũ�� : �������� �����ϰ� �˻�.
	*/

	int i, i2 = 0;
	int j, j2 = 0;
	int src_height, src_width;

	src_height = src.rows;
	src_width = src.cols;

	Mat window;
	Mat output(src_height, src_width, src.type(), Scalar(0));

	float m_height = src_height * sky_rate;

	for (i = m_height; i + window_size <= src_height; i = i + window_size / 10) {
		if (i + window_size >= src_height)
			i2 = src_height;
		else
			i2 = i + window_size;

		for (j = 0; j + window_size <= src_width; j = j + window_size / 10) {
			if (j + window_size >= src_width)
				j2 = src_width;
			else
				j2 = j + window_size;

			window = src(Range(i, i2), Range(j, j2));

			if (calcEdgeDirection3(window, 18, ratio, 8000)){
				output(Range(i, i2), Range(j, j2)) += Scalar(50, 50, 50);
			}
			else{
				output(Range(i, i2), Range(j, j2)) -= Scalar(50, 50, 50);
			}

		}
	}

	return output;

}



bool calcEdgeDirection3(Mat Origin, int NumBins, float _ratio, int threshold){

	Mat src, gray, sobel_x, sobel_y, result, canny, mask_x, mask_y;

	src = Origin.clone();

	//Dynamic allocation
	vector<vector<float>> direction(src.cols, vector<float>(src.rows, 0));

	int bin_degree = 180 / NumBins;

	vector<float> bin_hist(NumBins, 0);

	cvtColor(src, gray, CV_BGR2GRAY);

	Sobel(gray, sobel_x, CV_32FC1, 1, 0, -1);
	Sobel(gray, sobel_y, CV_32FC1, 0, 1, -1);

	Mat Mag(gray.size(), CV_32FC1);
	Mat Angle(gray.size(), CV_32FC1);

	//Canny(gray, canny, 50, 100);

	//sobel_x.copyTo(mask_x, canny);
	//sobel_y.copyTo(mask_y, canny);
	//
	//imshow("MASK_X", mask_x + mask_y);


	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);


	for (int i = 0; i < Angle.rows; i++){

		for (int j = 0; j < Angle.cols; j++){

			//cout << sobel_x.at<float>(i,j) << endl;
			//cout << Angle.at<float>(i, j) << endl;

			float tempangle = Angle.at<float>(i, j);

			if (tempangle >= 180)
				tempangle -= 180;

			int a = tempangle / bin_degree;

			float k = tempangle - (a*bin_degree);

			float r = k / bin_degree;

			float tempmag = Mag.at<float>(i, j);

			//			if (tempmag > 100)
			//printf("Angle = %f, Magnitude = %f, BIN_HIST = %d\n", tempangle, tempmag, a);

			bin_hist[a] += tempmag*(1 - r);

			if ((a + 1) * bin_degree >= 180){
				bin_hist[0] += tempmag*r;
			}
			else{
				bin_hist[a + 1] += tempmag*r;
			}


		}
	}



	float sum = 0;

	int max_bin = 0;

	float cur_max = 0;

	for (int z = 0; z < NumBins; z++){

	//	printf("NumBin[%d] (%d ~  %d) = %f\n", z, z*bin_degree, (z + 1)*bin_degree, bin_hist[z]);
		if (bin_hist[z] >= cur_max){
			max_bin = z;
			cur_max = bin_hist[z];
		}

		sum += bin_hist[z];

	}


	if (sum <= threshold)
		return false;

	//printf("SUM = %f\n", sum);


	vector<float> bin_average(NumBins, 0);

	for (int z = 0; z < NumBins; z++)
		bin_average[z] = bin_hist[z] / sum;

	int temp_bin = max_bin;

	if (max_bin == 0)
		temp_bin = 9;

	if (bin_average[max_bin] + MAX(bin_average[temp_bin - 1], bin_average[(max_bin + 1) % NumBins]) >= _ratio)
		return false;
	else
		return true;

}
