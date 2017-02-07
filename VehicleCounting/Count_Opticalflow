
//openCV
#include "cv.hpp"
#include "opencv2/opencv.hpp"
//C
#include "RoadDetection.h"
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>


#define PI 3.1416
using namespace std;
using namespace cv;

char keyboard;
int ifLRline(Point2f A, Point2f B, Point2f P);
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r);
Point2f A, B;
void callBackFunc(int event, int x, int y, int flags, void* userdata);

int main(void) {
	int Pnum = 0, Car_num = 0;
	char* videoFilename = "C:\\Users\\darke\\Desktop\\CV\\traffic.mp4";

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
	
	vector<Point2f> Center,after, before;
	vector<uchar> status;
	vector<float> err;
	
	A = Point(350, 280);
	B = Point(1000, 280);
	Point point1,point2;
	bool click=false;
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
			exit(EXIT_FAILURE);
		}
		latter_gray.copyTo(former_gray);
		cvtColor(frame, latter_gray, CV_BGR2GRAY);
		//update gray image

		fgbg(frame, fore);
		//fgbg.set("detectShadows", false);	
		fore = fore > 200;
		GaussianBlur(fore, fore, Size(7, 7), 0, 0);
		erode(fore, fore, getStructuringElement(MORPH_RECT, Size(10, 10)));
		dilate(fore, fore, getStructuringElement(MORPH_RECT, Size(10, 10)));

		//calcHist(frame, 1, { 0 }, Mat(), histo, 1, &numbers_bin, { 0, 255 });
		copy = fore.clone();

		findContours(copy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		Pnum = 0;
		after.clear();
		Center.clear();
		before.clear();
		for (int i = 0; i < contours.size(); i++) {

			if (contours[i].size() > 2500)
				drawContours(frame, contours, i, Scalar(0, 255, 0), CV_FILLED, 8, hierarchy);

			Rect rect = boundingRect(contours[i]);
			if (rect.area() > 3000) {
				rectangle(frame, rect.tl(), rect.br(), Scalar(0, 0, 255), 2, 8, 0);
				x = (rect.br().x - rect.tl().x) / 2;
				y = (rect.br().y - rect.tl().y) / 2;
				//circle(frame, Point((rect.tl().x) + x, (rect.tl().y) + y), 3, Scalar(255, 0, 0), 3, 8); //center
				x_box = (rect.tl().x) + x;
				y_box = (rect.tl().y) + y;
				Center.push_back((Point(x_box, y_box)));
				Pnum++;
			}
		} // 차량의 중심점

		if(B.x!=0 && B.y!=0 )
		line(frame, A, B, Scalar(0, 0, 255), 8, 8, 0);

		if (Pnum > 0) { //optical flow
			for (int i = 0; i < Pnum; i++) {
				circle(frame, Center[i], 5, Scalar(0, 255, 0), 3, 8);
			}

			calcOpticalFlowPyrLK(former_gray, latter_gray, Center, after, status, err, Size(25, 25),5);
			for (int i = 0; i < Pnum; i++) {
				if (status[i] == 0)
					continue;
				circle(frame, after[i], 3, Scalar(0, 0, 255), 3, 8);
				Point2f P;
				if (intersection(Center[i], after[i], A, B, P)) {
					if ((((A.x <= P.x) && (P.x <= B.x)) || ((B.x <= P.x) && (P.x <= A.x))) && (((A.y <= P.y) && (P.y <= B.y)) || ((B.y <= P.y) && (P.y <= A.y)))) {
						//o1,p1  -- o2, p2
						if (ifLRline(A, B, after[i]) * ifLRline(A, B, Center[i])  < 0 ) {
							Car_num++;
							cout << "Counted Car" << Car_num << endl;
							line(frame, A, B, Scalar(0, 255, 255), 10, 10, 0);
						}
					}
				}
			}
		}
		for (int i = 0; i < Pnum; i++)
			before.push_back(Center[i]);
		char tmp[20];

		sprintf(tmp, "Car num : %d", Car_num);
		putText(frame, tmp, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(10, 10, 10), 1, CV_AA);

		imshow("BACK", fore);
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
