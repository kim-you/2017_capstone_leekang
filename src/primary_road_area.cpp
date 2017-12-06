
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat calNonedgeArea(Mat src, int windowSize, float stride, float edgeRate) {

	/*
	Mat src :  원본 영상(에지처리후->2진화영상으로 변환된 영상이어야함.
	float sky_rate : 하늘에 해당하는 비율 (ex/ 0.3 : 상위 30%를 무시한다)
	int window_size : 검색윈도우의 크기 : 낮을수록 정밀하게 검색.
	float stride : 검색 윈도우의 이동 간격(1 = 윈도우사이즈만큼 / 0.5 = 윈도우사이즈의 반만큼 이동)
	*/

	float i, j;
	int height = src.rows;
	int	width = src.cols;

	Mat window;
	Mat output(height, width, src.type(), Scalar(0));

	int wstride = MAX(windowSize * stride, 1);

	for (i = 0; i + windowSize <= height; i += wstride) {

		for (j = 0; j + windowSize <= width; j += wstride) {

			window = src(Range(i, i + windowSize), Range(j, j + windowSize)); //윈도우 영역을 저장

			if (sum(window)[0] / 255 <= windowSize * windowSize * edgeRate)   // 윈도우 영역 내에 엣지가 정한 비율보다 낮다면
				output(Range(i, i + windowSize), Range(j, j + windowSize)) = Scalar(255);
		}
	}

	return output;

}


Mat findRoadBlob(Mat nonEdgeMap, Mat filtered, float sky_rate, float labLimit, float sigma, float sizeLimit) {

	float largestArea = 0;
	int largestIndex;

	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat temp = nonEdgeMap.clone();
	erode(temp, temp, Mat());

	//findContours(temp, contours, hierarchy, RETR_EXTERNAL, 2);
	findContours(temp, contours, hierarchy, 2, 2);

	float m_height = nonEdgeMap.rows * sky_rate;

	vector<float> contourarea(contours.size());
	vector<Moments> mu(contours.size());
	vector<Point2f> mc(contours.size());


	for (int i = 0; i < contours.size(); i++)
	{
		contourarea[i] = contourArea(contours[i]);
		mu[i] = moments(contours[i]);
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

		if (contourarea[i] > largestArea && mc[i].y > m_height) {
			largestArea = contourarea[i];
			largestIndex = i;
		}
	}

	temp = Scalar(0);
	drawContours(temp, contours, largestIndex, Scalar(255), -1, 8, hierarchy);

	Mat src_Lab;
	cvtColor(filtered, src_Lab, CV_BGR2Lab);
	Scalar stdmean, stddev;

	meanStdDev(src_Lab, stdmean, stddev, temp);

	float minSize = sizeLimit * sizeLimit;
	float L_limit = 0;

	Mat output = nonEdgeMap.clone();
	output = Scalar(0);

	for (int i = 0; i < contours.size(); i++) {

		if (contourarea[i] < minSize)
			continue;

		temp = Scalar(0);
		Scalar tempstdmean, tempstddev;
		drawContours(temp, contours, i, Scalar(255), -1, 8, hierarchy);
		meanStdDev(src_Lab, tempstdmean, tempstddev, temp);
		float diff = abs(stdmean[1] - tempstdmean[1]) + abs(stdmean[2] - tempstdmean[2]);
		if (diff < 1) L_limit = L_limit * 2;
		else L_limit = stddev[0] * sigma;

		if ((diff < labLimit) && tempstdmean[0] >(stdmean[0] - L_limit) && tempstdmean[0] < (stdmean[0] + L_limit))
			output += temp;
	}
	return output;
}
