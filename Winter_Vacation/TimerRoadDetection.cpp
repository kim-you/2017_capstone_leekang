/*---------------------------------------------------------\
이강 교수님 캡스톤 프로젝트 팀
CCTV 교통 혼잡도 측정.
이 프로그램은 CCTV을 이용, 배경을 구하는 프로그램.
따로 함수화 하진 않았습니다.
3초마다 한 번 씩 MOG2를 이용한 배경영상을 얻고
그 중 엣지가 최소화 된 것을 배경으로 사용합니다.
또한 기존 사용하던 Fore 영상을 개량, 
구해진 배경영상과 원본 영상의 차영상을 통해 Fore로 만드려는
시도 중 입니다.
\---------------------------------------------------------*/

#include "cv.hpp"
#include "opencv2/opencv.hpp"
#include "RoadDetection.h" //제작 헤더
#include <stdio.h>
#include <iostream>
#include <sstream>

#define SEC int(3) //How wait seconds for getting background Image

using namespace std;
using namespace cv;

int main(void){
	char keyboard;
	int min = 1000000;
	char* videoFilename = "C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic.mp4";
	int number = 0;
	int begin = 0;
	int cnt = 0;
	
	Mat box, box2, box3, box4;
	Mat edge_filter, frame_mask, frame_before;
	Mat copy, filter, copy2, copy3, diff;
	
	VideoCapture cap(videoFilename);
	
	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;

	int fps = cap.get(CV_CAP_PROP_FPS); //원본 영상의 FPS Get
	int frame_rate = fps;

	Mat frame, min_back, lab_back, hsv_back;
	Mat back, fore, gray, fore2;
	Mat Color_Mask, temp, temp2, temp3;
	Mat canny, trace, output, trace2, trace2_before;

	BackgroundSubtractorMOG2 fgbg;
	
	cap >> frame;
	
	keyboard = 0;
	
	while (1) {
		
		//read the current frame
		if (!cap.read(frame)) {
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}

		resize(frame, frame, cv::Size(640, 480)); //Resize for better quality of view.
		
		//cvtColor(frame, frame, CV_BGR2HSV);
		fgbg(frame, fore);
		//background
		fgbg.getBackgroundImage(back);
		//gray, binary
		fore = fore > 128;
		cvtColor(back, gray, CV_BGR2GRAY);
		GaussianBlur(gray, gray, Size(3, 3), 0, 0);
		GaussianBlur(fore, fore, Size(3, 3), 0, 0);

	//	erode(fore, fore, getStructuringElement(MORPH_RECT, Size(5, 5)));
	//	dilate(fore, fore, getStructuringElement(MORPH_RECT, Size(5, 5)));

		imshow("FORE", fore);

		//count edges
	
		if (cnt % (fps * SEC) == 0) {
			Canny(gray, canny, 15, 30);
			imshow("CANNY", canny);
			cnt = 0;
			copy = canny.clone();
			cout << countNonZero(copy) << "\n";
			if (countNonZero(copy) < min) {
				number++;
				min = countNonZero(copy);
				min_back = back.clone();
				char str[200];
				sprintf(str, "%d", number);
				putText(min_back, str, Point(10, 10), 1, 1, Scalar(0, 0, 255));
				cout << "-----Update-----\n";
				
				box = nonedge_area(canny, 0.3, 10);
				box3 = FindLargestArea(frame, box);

				//Input Lab Matrix && Largest Area's Mask.
				
				cvtColor(min_back, lab_back, CV_BGR2Lab);
				Scalar value = mean(lab_back, box3); // box3 -> Largest Area of Contour
				filter = roadFilter(lab_back, 1.2, box3);
				filter = filter > 128; // Filter -> LAB MASK's MATRIX
			
				//Input BGR Matrix && Largest Area's Mask.
				box4 = roadFilter2(min_back, 2.5, box3);
				box4 = box4 > 128; // box4 = RGB MASK's MATRIX

				//AND MASK FILTER&&BOX4
				bitwise_and(filter, box4, Color_Mask);
				
				imshow("ORIGIN", frame);
				setMouseCallback("ORIGIN", callBackFunc2, &frame); //Get Mouse Event For knowing Scalar Value.

			}
			cout << "MINIMUM " << min << "\n----------------\n" <<endl;

		}

		absdiff(frame, min_back, diff); // FRAME - BACKGROUND
		copy2 = frame.clone();
		
		/*findContours(diff, contours, hierarchy, 0, 1);
		for (int i = 0; i < contours.size(); i++){
				Rect rect = boundingRect(contours[i]);
				if (rect.area()>=500)
				rectangle(frame, rect.tl(), rect.br(), Scalar(0, 0, 255), 2, 8, 0);
		}
		frame.copyTo(frame_mask, Color_Mask);
		imshow("MASKS", frame_mask);
		if (frame_before.empty())
			frame_before = frame_mask.clone();
			race2=Extract_background(frame_before, frame_mask);
		
		trace2_before = trace2 + trace2_before;
		trace2_before = trace2_before > 128;
		
		copy3 = trace2_before.clone();
		bitwise_and(Color_Mask, trace2_before, Color_Mask);
		findContours(copy3, contours, hierarchy, 2, CV_CHAIN_APPROX_SIMPLE);
		for (int i = 0; i < contours.size(); i++){
				if (contours[i].size()>150)
				drawContours(frame, contours, i, Scalar(0, 255, 0), CV_FILLED, 4, hierarchy);
		}
		//print
		imshow("trace2", trace2_before);
		copy2.copyTo(copy3, Color_Mask);
		inRange(copy3, Scalar(255, 255, 255), Scalar(255, 255, 255), copy3);*/

		
		imshow("ORIGIN", frame);
		imshow("MASK", Color_Mask);
		imshow("BACK", min_back);
		imshow("Min", min_back);
		
		//GRAY Scale's Substraction For getting Foreground Image.
		cvtColor(frame, temp2, CV_BGR2GRAY);
		cvtColor(min_back, temp, CV_BGR2GRAY);
		absdiff(temp, temp2, temp3);
		temp3 += 2*temp3;
		temp3 = temp3 > 100;
		GaussianBlur(temp3, temp3, Size(3, 3), 0, 0);

		imshow("TEMP", temp3);
		
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

		frame_before = frame_mask.clone();
		
	}
}