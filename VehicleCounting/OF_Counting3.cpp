//openCV
#include "cv.hpp"
#include "opencv2/opencv.hpp"
//C
#include "RoadDetection.h"
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

void callBackFunc(int event, int x, int y, int flags, void* userdata);

int main(void) {
	char keyboard;
	int Pnum = 0, Car_num = 0;
	char* videoFilename = "C:\\Users\\darke\\Desktop\\CV\\traffic5.mp4";

	VideoCapture cap(videoFilename);

	int fps = cap.get(CV_CAP_PROP_FPS);
	int frame_rate = fps;
	int x, y, x_box, y_box;
	int count = 0;

	Mat frame, canny, fore, copy, histo, former_gray, latter_gray;
	Mat temporary;
	BackgroundSubtractorMOG2 fgbg;

	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;

	vector<Point2f> after, before, corners;
	vector<uchar> status;
	vector<float> err;

	A = Point(350, 280);
	B = Point(1000, 280);  // initial line(A-B)
	Point2f point1, point2, pass;
	bool click = false;

	namedWindow("ORIGIN", 1);
	if (!cap.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << videoFilename << endl;
		exit(EXIT_FAILURE);
	}

	setMouseCallback("ORIGIN", callBackFunc, (void*)(&point1));

	while (1) {
		//read the current frame
		if (!cap.read(frame)) {
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			cout << "Counted Car" << Car_num << endl; 
			exit(EXIT_FAILURE);
		}
		latter_gray.copyTo(former_gray);
		cvtColor(frame, latter_gray, CV_BGR2GRAY);
		//update gray image
		fgbg(frame, fore);
		fore = fore > 200;
		GaussianBlur(fore, fore, Size(7, 7), 0, 0);
		erode(fore, fore, getStructuringElement(MORPH_RECT, Size(10, 10)));
		dilate(fore, fore, getStructuringElement(MORPH_RECT, Size(10, 10)));
		//foreground mask smoothing
		copy = fore.clone();
		goodFeaturesToTrack(copy, corners, 1000, 0.01, 5);
		Mat black = frame.clone();
		cvtColor(black, black, CV_BGR2GRAY);
		black = Scalar(0);
		for(int i =0; i<corners.size();i++)
			circle(black, corners[i], 4, Scalar(255),7);
		//find corners from foreground by goodFeaturesToTrack
		black = black + copy;
		dilate(black, black, getStructuringElement(MORPH_RECT, Size(10, 10)));
		erode(black, black, getStructuringElement(MORPH_RECT, Size(10, 10)));
		imshow("corners", black);
		//smoothing corners+foreground
		findContours(black, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		drawContours(black, contours, -1, Scalar(255));
		//Find contour from corners+foreground
		Pnum = 0;
		vector<Moments> mu(contours.size());
		vector<Point2f> mc(contours.size());
		vector<Point2f> Center;
		for (int i = 0; i < contours.size(); i++){
			if (contourArea(contours[i]) < 2000)
				continue;
			else{
				mu[i] = moments(contours[i], false);
				mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
				circle(frame, mc[i], 5, Scalar(0, 255, 0), 3, 8);
				Center.push_back(mc[i]);
				Pnum++;
			}
		}//  Get the mass centers of contours and draw

		if (B.x != 0 && B.y != 0)
			line(frame, A, B, Scalar(0, 0, 255), 8, 8, 0);
		// draw line
		if (Pnum > 0) { //optical flow
			calcOpticalFlowPyrLK(former_gray, latter_gray, Center, after, status, err, Size(25, 25), 3);
			OpticalFlow_Count(Pnum, status, Car_num, frame, pass, after, Center, A, B);
		}

		char tmp[20];
		sprintf(tmp, "Car num : %d", Car_num);
		putText(frame, tmp, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(10, 10, 10), 1, CV_AA);

		imshow("BACK", black);
		//print
		imshow("ORIGIN", frame);

		if (keyboard == 32) {
			if (frame_rate == fps)
				frame_rate = 0;
			else if (frame_rate == 0)
				frame_rate = fps;
		}
		else if (keyboard == 27)
			break;
		
		keyboard = (char)waitKey(frame_rate);
	}
}

void callBackFunc(int event, int x, int y, int flags, void* userdata) {

	switch (event) {
	case EVENT_LBUTTONDOWN:
		A.x = x;
		A.y = y;
		B.x = 0;
		B.y = 0;
		break;

	case EVENT_LBUTTONUP:
		B.x = x;
		B.y = y;
		break;
	}
}
