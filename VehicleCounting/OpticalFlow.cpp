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

char keyboard;

int main(void){
	
	char* videoFilename = "C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic8.mp4";
	
	VideoCapture cap(videoFilename);

	int fps = cap.get(CV_CAP_PROP_FPS);
	int frame_rate = fps;
	int x, y, x_box, y_box;
	int count = 0;

	Mat frame, canny, fore, copy, histo;
	BackgroundSubtractorMOG2 fgbg;

	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;


	cap >> frame;

	resize(frame, frame, cv::Size(850,550), 0, 0, 0);

	if (!cap.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open video file: " << videoFilename << endl;
		exit(EXIT_FAILURE);
	}
	
	while (1) {
		
		//read the current frame
		if (!cap.read(frame)){
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}

		fgbg(frame, fore);
		fore = fore > 128;
		GaussianBlur(fore, fore, Size(7, 7), 0, 0);
		erode(fore, fore, getStructuringElement(MORPH_RECT, Size(10, 10)));
		dilate(fore, fore, getStructuringElement(MORPH_RECT, Size(10, 10)));
		
		//calcHist(frame, 1, { 0 }, Mat(), histo, 1, &numbers_bin, { 0, 255 });
		copy = fore.clone();
		
		findContours(copy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		for (int i = 0; i < contours.size(); i++){
			
			if (contours[i].size()>5000)
			drawContours(frame, contours, i, Scalar(0, 255, 0), CV_FILLED, 8, hierarchy);
			
			Rect rect = boundingRect(contours[i]);
			if (rect.area() > 7000){
				rectangle(frame, rect.tl(), rect.br(), Scalar(0, 0, 255), 2, 8, 0);
				x = (rect.br().x - rect.tl().x) / 2;
				y = (rect.br().y - rect.tl().y) / 2;
				circle(frame, Point((rect.tl().x)+x, (rect.tl().y)+y), 3, Scalar(255, 0, 0), 3, 8);
				x_box = (rect.tl().x) + x;
				y_box = (rect.tl().y) + y;
				
				if (((x_box > 271) && (y_box > 122)) && ((x_box < 832) && (y_box < 138))){
					count++;
					cout << "\n" << count;
					if (rect.area() > 20000)

						cout << " Wow Big\n";
				}
			}
		}
		
		line(frame, Point(271, 130), Point(832, 130), Scalar(0, 0, 255), 8, 8, 0);
		
		imshow("BACK", fore);
		//print
		imshow("ORIGIN", frame);

		setMouseCallback("ORIGIN", callBackFunc, &frame);
		

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