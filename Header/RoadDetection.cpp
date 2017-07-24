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

using namespace cv;

int counts_number = 1;

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

Mat nonedge_area(Mat src, float sky_rate, int window_size) {
	/*
	Mat src :  원본 영상(에지처리후->2진화영상으로 변환된 영상이어야함.
	float sky_rate : 하늘에 해당하는 비율 (ex/ 0.3 : 상위 30%를 무시한다)
	int window_size : 윈도우의 크기 : 낮을수록 정밀하게 검색.
	*/

	int i, i2 = 0;
	int j, j2 = 0;
	int src_height, src_width;

	src_height = src.rows;
	src_width = src.cols;

	Mat window;
	Mat output(src_height, src_width, src.type(), Scalar(0));

	float m_height = src_height * sky_rate;

	for (i = m_height; i + window_size <= src_height; i = i + window_size) {
		if (i + window_size >= src_height)
			i2 = src_height;
		else
			i2 = i + window_size;

		for (j = 0; j + window_size <= src_width; j = j + window_size) {
			if (j + window_size >= src_width)
				j2 = src_width;
			else
				j2 = j + window_size;

			window = src(Range(i, i2), Range(j, j2));
			if (sum(window) == Scalar(0))
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
	printf("LAB MEAN %lf %lf %lf\n", mean_v[0], mean_v[1], mean_v[2]);
	printf("LAB DEV %lf %lf %lf\n", dev_v[0], dev_v[1], dev_v[2]);
	printf("SIGMA %lf %lf %lf\n", sigma_v[0], sigma_v[1], sigma_v[2]);

	if ((sigma_v[1] + sigma_v[2]) <= 20){
		sigma_v[1] = 7;
		sigma_v[2] = 10;
	}

	printf("SIGMA_FIX %lf %lf %lf\n", sigma_v[0], sigma_v[1], sigma_v[2]);
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
	printf("BGR MEAN %lf %lf %lf\n", mean_v[0], mean_v[1], mean_v[2]);
	printf("BGR DEV %lf %lf %lf\n", dev_v[0], dev_v[1], dev_v[2]);
	printf("SIGMA %lf %lf %lf\n", sigma_v[0], sigma_v[1], sigma_v[2]);

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
		sprintf(str, "%d", counts_number);
		putText(src, str, Point(x - 7, y - 10), 1, 1, Scalar(0, 0, 255));

		imshow("ORIGIN", src);

		counts_number++;

		break;
	}

}

Mat LabBgrMask(Mat origin, Mat background){

	Mat back, canny, gray;
	Mat box, box3, lab_back, filter, box4, Color_Mask;

	back = background.clone();

	cvtColor(back, gray, CV_RGB2GRAY);
	GaussianBlur(gray, gray, Size(7, 7), 0, 0);
	Canny(gray, canny, 15, 25, 3);
	imshow("CANNY", canny);
	box = nonedge_area(canny, 0.3, 20);
	imshow("BOX", box);
	box3 = FindLargestArea(origin, box); // this is the mask

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
	//string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\MachineLearning\\cascade_Test\\cascade.xml";
	//LBP
	string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\MachineLearning\\cascade\\cascade.xml";
	//HOG
	//string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\MachineLearning\\cascade_Haar\\cascade.xml";
	//SVM
	//string cascadeName = "C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\MachineLearning\\trainedSVM.xml";
	if (!(path.empty())) string cascadeName = path;


	static CascadeClassifier detector;

	if (flag == 0){
		if (!detector.load(cascadeName))
		{
			printf("ERROR: Could not load classifier cascade\n");
			return;
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
		ROI = frame(ROI_rect);



		/* code block 2 */
		vector<Rect> found;
		detector.detectMultiScale(ROI, found, scale_step, gr_thr, 0, min_obj_sz_step, max_obj_sz_step);


		//add offset
		for (int i = 0; i < (int)found.size(); i++) {
			found[i].x += w*0.1;
			found[i].y += h*0.1;
		}
		//draw rectangles
		for (int i = 0; i < (int)found.size(); i++)
		{
			rectangle(frame, found[i], Scalar(0, 255, 255), 3);
		}

		// processing time check (fps)
		::QueryPerformanceCounter((_LARGE_INTEGER*)&finish);
		double fps = freq / double(finish - start + 1);
		char fps_str[20];
		sprintf_s(fps_str, 20, "FPS: %.1lf", fps);
		putText(frame, fps_str, Point(5, 35), FONT_HERSHEY_SIMPLEX, 1., Scalar(0, 0, 0), 2);


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