#include "cv.hpp" //���⿡ �ʿ��� �� �� ����
#include "opencv2/opencv.hpp" //�̷��Ը� �ϸ� �� ��� �� ���

using namespace std;
using namespace cv;

int main(){

	Mat src, add, back;
	Mat canny,sobel;
	VideoCapture capture("C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\TrafficExample\\traffic.mp4");
	int key;
	int frame_rate = 5;
	Mat pre;

	BackgroundSubtractorMOG2 fgbg;

	capture >> src;

	while (capture.read(src)){
		fgbg(src, back);
		fgbg.getBackgroundImage(pre);
		imshow("ORG", src);
		
		Canny(pre, canny, 50, 100, 3);
		
		//imshow("BACK", back);
		imshow("BACKGROUND", canny);


		key = waitKey(frame_rate);

		if (key == 32){
			if (frame_rate == 5)
				frame_rate = 0;
			else if (frame_rate == 0)
				frame_rate = 5;

		}
		else if (key == 27)
			break;
	}
}