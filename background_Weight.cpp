#include "cv.hpp" //여기에 필요한 거 다 있음
#include "opencv2/opencv.hpp" //이렇게만 하면 다 뜬다 다 뜬다

using namespace std;
using namespace cv;

int main(){
	VideoCapture inputVideo("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic.mp4");
	if (!inputVideo.isOpened())
	{
		cout << "Can not open inputVideo !!!" << endl;
		return -1;
	}
	Size size = Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
	cout << "Size = " << size << endl;

	int fps = (int)(inputVideo.get(CV_CAP_PROP_FPS));
	cout << "fps = " << fps << endl;

	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5)); // MORPH_ELLIPSE

	double alpha = 0.05;
	int nTh = 50;
	int frameNum = -1;
	int delay = 1000 / fps;
	Mat frame, grayImage;
	Mat avgImage, diffImage, mask, canny;
	for (;;)
	{
		inputVideo >> frame;
		if (frame.empty())
			break;
		frameNum++;	
		//cout << "frameNum: " << frameNum << endl;
		cvtColor(frame, grayImage, COLOR_BGR2GRAY);
		GaussianBlur(grayImage, grayImage, Size(5, 5), 0.5);
		if (frameNum == 0)
		{
			avgImage = grayImage;
			continue;
		}
		avgImage.convertTo(avgImage, CV_32F);
		grayImage.convertTo(grayImage, CV_32F);
		absdiff(grayImage, avgImage, diffImage);
		threshold(diffImage, mask, nTh, 255, THRESH_BINARY);
		mask.convertTo(mask, CV_8U);

		erode(mask, mask, kernel, Point(-1, -1), 2);
		dilate(mask, mask, kernel, Point(-1, -1), 3);

		//imshow("mask", mask);
		bitwise_not(mask, mask);
		accumulateWeighted(grayImage, avgImage, alpha, mask);

		avgImage.convertTo(avgImage, CV_8U);
		//imshow("avgImage", avgImage);
		imshow("ORIGIN", frame);

		Mat canny;

		Canny(avgImage, canny, 50, 100, 3);
		imshow("Canny", canny);

		int cKey = waitKey(delay);
		
		if (cKey == 32){
			if (delay == 1000 / fps)
				delay = 0;
			else if (delay == 0)
				delay = 1000 / fps;
		}
		
		else if (cKey == 27) break;
		
	}
	return 0;
}