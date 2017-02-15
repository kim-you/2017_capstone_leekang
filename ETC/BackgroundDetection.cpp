#include "cv.hpp" //여기에 필요한 거 다 있음
#include "opencv2/opencv.hpp" //이렇게만 하면 다 뜬다 다 뜬다
#include "RoadDetection.h"

using namespace std;
using namespace cv;

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>

using namespace std;
using namespace cv;

Mat nonedge_area(Mat src, float sky_rate, int window_size);

int main(void) {
	Mat frame, poly, copy;
	Mat back, fore, canny, gray;
	Mat Final;
	int key, frame_rate=30;
	int blue=0, green=0, red=0;
	
	//VideoCapture cap("example2.avi");
	VideoCapture cap("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic.mp4");
	int start = 0;
	BackgroundSubtractorMOG2 bg;

	while (1) {
		cap >> frame;

		if (!frame.data) {
			cout << "end" << endl;
			break;
		}
		//foreground 계산
		bg(frame, fore);
		bg.getBackgroundImage(back);

		erode(fore, fore, Mat());
		dilate(fore, fore, Mat());
		/*
		if (start == 0) {
		oldfore = fore.clone();
		start = 1;
		}
		else
		oldfore += fore.clone(); //oldfore는 그냥 차들이 지나간 영역을 계속 더하는 역할.
		*/
		cvtColor(back, gray, CV_BGR2GRAY);
		Canny(gray, canny, 10, 30, 3);
		

		threshold(canny, canny, 127, 255, CV_THRESH_BINARY);
		
		poly = nonedge_area(canny, 0.3, 15);
		
		Final = FindLargestArea(back, poly);

		Scalar value = mean(back, Final);

		printf("Blue = %d, Green = %d, Red = %d\n", int(value.val[0]), int(value.val[1]), int(value.val[2]));

//		for (int i = 0; i < 3; i++)
//			printf("The result[%d] = %d", i, result[i]);

		Final = roadFilter(int(value.val[0]), int(value.val[1]), int(value.val[2]), 1.5, back);

		imshow("Frame", frame);
		//imshow("Canny", canny);
		imshow("Background", back);
	//	imshow("COPY", copy);
		imshow("final", Final);

		key = waitKey(frame_rate);

		if (key == 27)
			break;
		
		else if (key == 32){
			if (frame_rate == 30)
				frame_rate = 0;
			else if (frame_rate == 0)
				frame_rate = 30;
		}

	}

	return 0;
}


