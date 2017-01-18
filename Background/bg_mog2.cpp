#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>

using namespace std;
using namespace cv;

int main(void) {
	Mat frame;
	Mat back, fore, oldfore;
	VideoCapture cap("example.mp4");
	int start = 0;
	BackgroundSubtractorMOG2 bg;
	namedWindow("Frame");
	namedWindow("Background");
	namedWindow("Foreground");

	namedWindow("oldForeground");
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
		
		if (start == 0) {
			oldfore = fore.clone();
			start = 1;
		}
		else
			oldfore += fore.clone(); //oldfore는 그냥 차들이 지나간 영역을 계속 더하는 역할.
		
		imshow("Frame", frame);
		imshow("Foreground", fore);
		imshow("Background", back);
		imshow("oldForeground", oldfore);

		if (waitKey(1) >= 0) break;
	}
	return 0;
}

