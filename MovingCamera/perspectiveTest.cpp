#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all
#include "RoadDetection.h"
#include <deque>

using namespace std;
using namespace cv;


#define ROTATION 3 //Define how many rotation it is
#define FRAMERATE 30 //Define fps
#define AV_WEIGHT float(0.2)
#define DIST_SIZE 20

Point TopLeft(235,170);
Point TopRight(369,170);
Point BottomLeft(24,322);
Point BottomRight(589,342);

void callBackFunc2(int event, int x, int y, int flags, void* userdata);

int main(){
	int key = 0;
	int frame_rate = 30;
	Size _resize(600, 400);


	Mat src = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\Image3.jpg");

	resize(src, src, _resize);


	if (!src.data){
		printf("NO IMAGE\n");
		return -1;
	}



	imshow("ORIGIN", src);
	setMouseCallback("ORIGIN", callBackFunc2, &src); //Get Mouse Event For knowing Scalar Value.
	
	vector<Point> rect;
	rect.push_back(TopLeft);
	rect.push_back(TopRight);
	rect.push_back(BottomRight);
	rect.push_back(BottomLeft);

	double w1 = sqrt(pow(BottomRight.x - BottomLeft.x, 2) + pow(BottomRight.y - BottomLeft.y, 2));
	double w2 = sqrt(pow(TopRight.x - TopLeft.x, 2) + pow(TopRight.y - TopLeft.y, 2));
	double h1 = sqrt(pow(TopRight.x - BottomRight.x, 2) + pow(TopRight.y - BottomRight.y, 2));
	double h2 = sqrt(pow(TopLeft.x - BottomLeft.x, 2) + pow(TopLeft.y - BottomLeft.y, 2));

	double maxWidth = (w1 < w2) ? w2 : w1;
	double maxLength = (h1 < h2) ? h2 : h1;

	Point2f source[4], out[4];
	source[0] = Point2f(TopLeft.x, TopLeft.y);
	source[1] = Point2f(TopRight.x, TopRight.y);
	source[0] = Point2f(BottomLeft.x, BottomLeft.y);
	source[0] = Point2f(BottomRight.x, BottomRight.y);

	out[0] = Point2f(0, 0);
	out[1] = Point2f(maxWidth - 1, 0);
	out[2] = Point2f(maxWidth - 1, maxLength - 1);
	out[3] = Point2f(0, maxLength - 1);

	Mat test = getPerspectiveTransform(source, out);
	Mat output;

	warpPerspective(src, output, test, Size(maxWidth, maxLength));
	imshow("TEST", output);

	waitKey(0);
	//while (capture.read(src)){

	//

	//	key = waitKey(frame_rate);

	//	if (key == 27 || key == 'q')
	//		break;

	//	if (key == 32){
	//		if (frame_rate == FRAMERATE)
	//			frame_rate = 0;
	//		else if (frame_rate == 0)
	//			frame_rate = FRAMERATE;
	//	}
	//}

	return 0;

}

void callBackFunc2(int event, int x, int y, int flags, void* userdata){
	static int counts_number = 1;
	/* When Mouse Click, Get Scalar Values on Clicked Point */

	Mat src = *(Mat*)userdata;
	Mat src2, src3, src4, src5, src6;

	cvtColor(src, src2, CV_BGR2Lab);
	cvtColor(src, src3, CV_BGR2HSV);
	cvtColor(src, src4, CV_BGR2YCrCb);
	cvtColor(src, src5, CV_BGR2XYZ);

	switch (event){

	case EVENT_LBUTTONDOWN:

		printf("%d : X = %d, Y = %d\n", counts_number, x, y);
		printf("BGR Screen: Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n",
			src.at<Vec3b>(y, x)[0], src.at<Vec3b>(y, x)[1], src.at<Vec3b>(y, x)[2]);
		printf("-----------------------------------------------------------------\n");

		rectangle(src, Point(x - 5, y - 5), Point(x + 5, y + 5), Scalar(0, 0, 255), 1, 8);

		char str[200];
		sprintf_s(str, "%d", counts_number);
		putText(src, str, Point(x - 7, y - 10), 1, 1, Scalar(0, 0, 255));

		imshow("ORIGIN", src);

		counts_number++;

		break;
	}

}