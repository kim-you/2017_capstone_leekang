#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all
#include <vector>

using namespace std;
using namespace cv;

int main(){
	int i = 0, count = 0;
	double maxcontour = 0;
	Mat src = imread("C:\\Users\\Administrator\\Desktop\\Study\\4�г�\\������\\OpenCV\\Coin.jpg", 1);
	Mat range, canny, copy, origin;
	int locate[100][100];
	int x = 0, y = 0;
	int nBlue = 0, nGreen = 0, nRed = 0;
	int min_Blue = 256, min_Green = 256, min_Red = 256;

	if (!src.data){
		printf("NO IMAGE\n");
		return -1;
	}
	origin = src.clone();
	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;

	//	blur(src, canny, Size(3, 3), Point(-1, 1), 2);
	
	Canny(src, canny, 200, 400);
	
	Mat morph;
	Mat kernel = getStructuringElement(0, Size(7, 7));
	morphologyEx(canny, morph, MORPH_CLOSE, kernel);//close -> ��â 1 ħ�� 1 

	copy = morph.clone();

	findContours(copy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	for (i = 0; i < contours.size(); i++){
		printf("%d = %lf\n", i, contourArea(contours[i]));

		if (contourArea(contours[i])>maxcontour){
			maxcontour = contourArea(contours[i]);
			count = i;
		}

	}
	
	drawContours(src, contours, count, Scalar(0, 0, 255), CV_FILLED, 8, hierarchy);
	
	for (x = 0; x<src.rows; x++){
		for (y = 0; y<src.cols; y++){
			if (src.at<cv::Vec3b>(x, y)[0] == 0 && src.at<cv::Vec3b>(x, y)[1] == 0 && src.at<cv::Vec3b>(x, y)[2] == 255){
				nBlue = origin.at<cv::Vec3b>(x-1, y)[0];
				nGreen = origin.at<cv::Vec3b>(x-1, y)[1];
				nRed = origin.at<cv::Vec3b>(x-1, y)[2];
			}
		}
	}
	drawContours(src, contours, count, Scalar(nBlue, nGreen, nRed), CV_FILLED, 8, hierarchy);

	printf("Blue = %d, Green = %d, Red = %d", nBlue, nGreen, nRed);
	//blur(src, normal, 3, Point(-1, 1), 3);
	
	imshow("NORMAL", src);
	imshow("ORIGIN", origin);
	//	imshow("RANGE", range);
	imshow("MORPH", morph);
	imshow("CONTOUR", copy);
	//	imshow("COPY", copy);
	waitKey(0);


}