#include "cv.hpp" //여기에 필요한 거 다 있음
#include "opencv2/opencv.hpp" //이렇게만 하면 다 뜬다 다 뜬다

using namespace std;
using namespace cv;

int main(){
	Mat src, weight, back;
	VideoCapture capture("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\traffic.mp4");
	int key;
	int frame_rate = 30;

	BackgroundSubtractorMOG fgbg;

	capture >> src;

	while (capture.read(src)){
		fgbg(src, back, (0.1));

		imshow("ORG", src);
		imshow("BACK", back);

		key = waitKey(frame_rate);

		if (key == 27)
			break;
	}
}