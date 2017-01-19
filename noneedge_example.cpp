#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include "nonedge_area.cpp"

using namespace std;
using namespace cv;

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

int main(void) {
	Mat frame, poly;
	Mat back, fore, canny, gray;

	//VideoCapture cap("example2.avi");
	VideoCapture cap("example.mp4");
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
	
		poly = nonedge_area(canny, 0.3, 10);

		imshow("poly", poly);
		imshow("Frame", frame);
		imshow("Canny", canny);
		imshow("Background", back);
		
		if (waitKey(1) >= 0) break;
	}
	return 0;
}


