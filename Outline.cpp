#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all
#include <vector>

using namespace std;
using namespace cv;

int main(){
	int i = 0, count = 0;
	double maxcontour = 0;
	Mat src = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\lena.png", 1);
	Mat range, canny, copy, origin;
	int locate[100][100];
	int x = 0, y = 0;
	int nBlue = 0, nGreen = 0, nRed = 0;
	int min_Blue = 256, min_Green = 256, min_Red = 256;
	int max_Blue = -1, max_Green = -1, max_Red = -1;
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
	morphologyEx(canny, morph, MORPH_CLOSE, kernel);//close -> 팽창 1 침식 1 

	copy = morph.clone();

	findContours(copy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	for (i = 0; i < contours.size(); i++){
		printf("%d = %lf\n", i, contourArea(contours[i]));

		if (contourArea(contours[i])>maxcontour){
			maxcontour = contourArea(contours[i]);
			count = i;
		}

	}
	
	drawContours(src, contours, count, Scalar(93, 85, 199), CV_FILLED, 8, hierarchy);
	
	for (x = 0; x<src.rows; x++){
		for (y = 0; y<src.cols; y++){
			if (src.at<cv::Vec3b>(x, y)[0] == 0 && src.at<cv::Vec3b>(x, y)[1] == 0 && src.at<cv::Vec3b>(x, y)[2] == 255){
				if (nBlue < min_Blue && nBlue > max_Blue){
					nBlue = origin.at<cv::Vec3b>(x - 1, y - 1)[0];
					min_Blue = nBlue;
				}
				if (nGreen < min_Green){
					nGreen = origin.at<cv::Vec3b>(x - 1, y - 1)[1];
					min_Green = nGreen;
				}
				if (nRed < min_Red){
					nRed = origin.at<cv::Vec3b>(x - 1, y - 1)[2];
					min_Red = nRed;
				}
			}
		}
	}
	printf("Blue = %d, Green = %d, Red = %d", min_Blue, min_Green, min_Red);
	//blur(src, normal, 3, Point(-1, 1), 3);
	
	imshow("NORMAL", src);
	imshow("ORIGIN", origin);
	//	imshow("RANGE", range);
	imshow("MORPH", morph);
	imshow("CONTOUR", copy);
	//	imshow("COPY", copy);
	waitKey(0);


}