//openCV
#include "cv.hpp"
#include "opencv2/opencv.hpp"
//C
#include "RoadDetection.h"
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

#define SEC int(3)

using namespace std;
using namespace cv;

char keyboard;

int main(void){
	int min = 1000000;
	char* videoFilename = "C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic.mp4";
	int number = 0;
	int begin = 0;
	int cnt = 0;
	Mat box, box2, box3, box4;
	Mat edge_filter;
	Mat copy, filter;
	VideoCapture cap(videoFilename);
	
	int fps = cap.get(CV_CAP_PROP_FPS);
	int frame_rate = fps;

	Mat frame, min_back, lab_back, hsv_back;
	Mat back, fore, gray, fore2;
	Mat Color_Mask;
	Mat canny, trace, output, trace2;
	BackgroundSubtractorMOG2 fgbg;
	//BackgroundSubtractorMOG fgbg2;
	
	cap >> frame;
	
	//resize(frame, frame, cv::Size(850,550), 0, 0, 1);
	
	if (!cap.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << videoFilename << endl;
		exit(EXIT_FAILURE);
	}
	
	keyboard = 0;
	
	while (1) {
		
		//read the current frame
		if (!cap.read(frame)) {
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}
		//foreground
		//fgbg2(frame, fore2);
		fgbg(frame, fore);
		//background
		fgbg.getBackgroundImage(back);
		//gray, binary
		fore = fore > 128;
		cvtColor(back, gray, CV_RGB2GRAY);
		GaussianBlur(gray, gray, Size(3, 3), 0, 0);
		GaussianBlur(fore, fore, Size(3, 3), 0, 0);
		//GaussianBlur(fore2, fore2, Size(3, 3), 0, 0);
		
		if (begin++ == 0){
			trace = fore.clone();
			//trace2 = fore2.clone();
		}
		else{
			trace = trace + fore;
			//trace2 = trace2 + fore2;
		}

		
		//count edges
	
		if (cnt % (fps * SEC) == 0) {
			Canny(gray, canny, 15, 25, 3);
			cnt = 0;
			copy = canny.clone();
			cout << countNonZero(copy);
			if (countNonZero(copy) < min) {
				number++;
//				trace2 = trace2 > 128;
				min = countNonZero(copy);
				min_back = back.clone();
				char str[200];
				sprintf(str, "%d", number);
				putText(min_back, str, Point(10, 10), 1, 1, Scalar(0, 0, 255));
				cout << "   갱-신";
				cvtColor(min_back, box2, CV_BGR2GRAY);
				box2 = box2 > 128;
				box = nonedge_area(box2, 0.3, 20);
				//edge_filter = box.clone(); -> This is for edge filter, not good.
				box3 = FindLargestArea(frame, box);
				cvtColor(min_back, lab_back, CV_BGR2Lab);
				Scalar value = mean(lab_back, box3);
				printf("\nLab : Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n", int(value.val[0]), int(value.val[1]), int(value.val[2]));
				filter = roadFilter(int(value.val[0]), int(value.val[1]), int(value.val[2]), 1.2, lab_back);
				filter = filter > 128;
				//cvtColor(min_back, hsv_back, CV_BGR2HSV);
				Scalar value2 = mean(min_back, box3);
				printf("\nHSV : Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n", int(value2.val[0]), int(value2.val[1]), int(value2.val[2]));
				box4 = roadFilter2(int(value2.val[0]), int(value2.val[1]), int(value2.val[2]), 1.2, min_back);
				bitwise_and(filter, box4, Color_Mask);
				//bitwise_and(Color_Mask, trace2, Color_Mask);
				//bitwise_and(Color_Mask, edge_filter, Color_Mask);
				//frame.copyTo(Color_Mask, filter);
				imshow("ORIGIN", frame);
				setMouseCallback("ORIGIN", callBackFunc, &frame);
			}
			cout << "   " << min << endl;

		}

		//print
		imshow("ORIGIN", frame);
		imshow("MASK", Color_Mask);
		//imshow("TRACE", trace);
		//      imshow("Background", back);
		imshow("Min", min_back);
	//	imshow("LABFILTER", filter);
		//imshow("HSVFILTER", box4);
		//imshow("MOG1", trace2);
		
		if (keyboard == 32) {
			if (frame_rate == fps)
				frame_rate = 0;
			else if (frame_rate == 0)
				frame_rate = fps;
		}
		else if (keyboard == 27)
			break;

		keyboard = (char)waitKey(frame_rate);
		cnt++;
	}
}