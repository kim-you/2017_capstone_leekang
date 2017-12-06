
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat neighb_filter(Mat src, int ksize, double weight, int winsize) {

	Mat dst;
	Mat kernel = Mat::ones(ksize, ksize, CV_32F) / ((float)(ksize*ksize) - 1);
	kernel.at<float>(2, 2) = 0;

	filter2D(src, dst, -1, kernel);

	dst = dst*0.3 + src;
	morphologyEx(dst, dst, MORPH_DILATE, Mat());

	return dst;
}

Mat mergeMat(Size resolution, Mat den, double denw, Mat angle, double anglew, Mat color, double colorw, int winsize, float threshold) {

	Mat total(Size(den.cols, den.rows), CV_32FC1);
	//addWeighted(den, denw, angle, anglew, 0, total);
	total = den * denw + angle *anglew + color*colorw;
	Mat result = neighb_filter(total, 3, 1, winsize);

	Mat output(resolution, CV_8UC1);
	for (int j = 0; j < result.rows; j++) {
		for (int i = 0; i < result.cols; i++)
		{
			float rate = result.at<float>(j, i);
			Rect r = Rect(i*winsize, j*winsize, winsize, winsize);
			Scalar color = Scalar(rate * 255, rate * 255, rate * 255);
			rectangle(output, r, color, -1);
		}
	}
	output = output > 255 * threshold;
	return output;
}

vector<Mat> bg_edgeangle(int size, Mat src) {

	int width = src.cols;
	int height = src.rows;
	vector<Mat> histBGangle;
	Mat gray, sobel_x, sobel_y;
	cvtColor(src, gray, CV_BGR2GRAY);

	Sobel(gray, sobel_x, CV_32FC1, 1, 0);
	Sobel(gray, sobel_y, CV_32FC1, 0, 1);

	Mat Mag(gray.size(), CV_32FC1);
	Mat Angle(gray.size(), CV_32FC1);
	Mat edge_angle(gray.size(), CV_32FC1);

	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);

	for (int i = 0; i < Angle.rows; i++) {

		for (int j = 0; j < Angle.cols; j++) {

			float tempangle = Angle.at<float>(i, j);
			float tempmag = Mag.at<float>(i, j);
			if (tempmag > 50) {
				if (tempangle >= 180) {
					tempangle -= 180;
					edge_angle.at<float>(i, j) = tempangle;
				}
				else {
					edge_angle.at<float>(i, j) = Angle.at<float>(i, j);
				}
			}
		}
	}
	int h_bins = 5;
	int histSize[] = { h_bins };
	float h_ranges[] = { 0, 180 };
	const float* ranges[] = { h_ranges };
	int channels[] = { 0 };

	for (int j = 0; j + size <= height; j += size) {
		for (int i = 0; i + size <= width; i += size) {
			Rect tempR = Rect(i, j, size, size);
			Mat temp = edge_angle(tempR);
			Mat hist;
			calcHist(&temp, 1, channels, Mat(), hist, 1, histSize, ranges);
			histBGangle.push_back(hist);
		}
	}
	return histBGangle;
}


Mat comp_edgeangle(int size, Mat src, double mag, vector<Mat> histBGangle) {

	int width = src.cols;
	int height = src.rows;
	Mat a = src.clone();
	Mat gray, sobel_x, sobel_y;
	cvtColor(src, gray, CV_BGR2GRAY);

	Sobel(gray, sobel_x, CV_32FC1, 1, 0);
	Sobel(gray, sobel_y, CV_32FC1, 0, 1);

	Mat Mag(gray.size(), CV_32FC1);
	Mat Angle(gray.size(), CV_32FC1);
	Mat edge_angle(gray.size(), CV_32FC1);

	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);

	for (int i = 0; i < Angle.rows; i++) {

		for (int j = 0; j < Angle.cols; j++) {
			float tempmag = Mag.at<float>(i, j);
			float tempangle = Angle.at<float>(i, j);

			if (tempmag > 50) {
				if (tempangle >= 180) {
					tempangle -= 180;
					edge_angle.at<float>(i, j) = tempangle;
				}
				else {
					edge_angle.at<float>(i, j) = Angle.at<float>(i, j);
				}
			}
		}
	}
	int h_bins = 5;
	int histSize[] = { h_bins };
	float h_ranges[] = { 0, 180 };
	const float* ranges[] = { h_ranges };
	int channels[] = { 0 };
	Mat output(Size(width / size, height / size), CV_32FC1);
	int count = 0;
	int x, y = 0;

	for (int j = 0; j + size <= height; j += size, y++) {
		x = 0;
		for (int i = 0; i + size <= width; i += size, x++) {
			Rect tempR = Rect(i, j, size, size);
			Mat temp = edge_angle(tempR);
			Mat hist;
			calcHist(&temp, 1, channels, Mat(), hist, 1, histSize, ranges);
			float rate = compareHist(histBGangle[count], hist, 3);
			output.at<float>(y, x) = pow(rate, 1);


			count++;
		}
	}
	return output;
	imshow("anglehist", a);
}


vector<int> bg_edgeden(int size, Mat src) {

	int width = src.cols;
	int height = src.rows;
	Mat canny;
	vector<int> BGedgenum;
	Canny(src, canny, 60, 80);

	for (int j = 0; j + size <= height; j += size) {
		for (int i = 0; i + size <= width; i += size) {
			Rect tempR = Rect(i, j, size, size);
			Mat temp = canny(tempR);
			int edgenum = sum(temp)[0] / 255;
			BGedgenum.push_back(edgenum);
		}
	}
	return BGedgenum;
}


Mat comp_edgeden(int size, Mat src, double mag, vector<int> BGedgenum) {

	int width = src.cols;
	int height = src.rows;
	Mat a = src.clone();
	Mat canny;
	int pixnum = size * size;
	Canny(src, canny, 60, 80);
	Mat output(Size(width / size, height / size), CV_32FC1);
	int count = 0;
	int x, y = 0;
	for (int j = 0; j + size <= height; j += size, y++) {
		x = 0;
		for (int i = 0; i + size <= width; i += size, x++) {
			Rect tempR = Rect(i, j, size, size);
			Mat temp = canny(tempR);
			int edgenum = sum(temp)[0] / 255;

			float ratio = (float)(abs(BGedgenum[count] - edgenum) / (float)pixnum);
			//Scalar Color = Scalar(0,0,pow(255 *ratio,1.2));
			output.at<float>(y, x) = pow(ratio, mag);

			count++;
		}
	}

	return output;
}



vector<Mat> bg_colorhist(int size, Mat src) {


	int width = src.cols;
	int height = src.rows;
	Mat labsrc;
	vector<Mat> histBGcolor;
	cvtColor(src, labsrc, CV_BGR2Lab);

	int histSize[] = { 3, 5, 5 };
	float Lranges[] = { 0, 255 };
	float abranges[] = { 0, 255 };
	const float* ranges[] = { Lranges, abranges, abranges };
	int channels[] = { 0, 1 , 2 };
	bool uniform = true, accumulate = false;

	for (int j = 0; j + size <= height; j += size) {
		for (int i = 0; i + size <= width; i += size) {
			Rect tempR = Rect(i, j, size, size);
			Mat temp = labsrc(tempR);
			Mat hist;
			calcHist(&temp, 1, channels, Mat(), hist, 3, histSize, ranges, uniform, accumulate);

			histBGcolor.push_back(hist);
		}
	}
	return histBGcolor;
}


Mat comp_colorhist(int size, Mat src, double mag, vector<Mat> histBGcolor) {

	Mat a = src.clone();
	Mat labsrc;
	int width = src.cols;
	int height = src.rows;

	cvtColor(src, labsrc, CV_BGR2Lab);

	int histSize[] = { 3, 5, 5 };
	float Lranges[] = { 0, 255 };
	float abranges[] = { 0, 255 };
	const float* ranges[] = { Lranges, abranges, abranges };
	int channels[] = { 0, 1, 2 };
	bool uniform = true, accumulate = false;
	Mat output(Size(width / size, height / size), CV_32FC1);
	int count = 0;
	int x, y = 0;
	for (int j = 0; j + size <= height; j += size, y++) {
		x = 0;
		for (int i = 0; i + size <= width; i += size, x++) {
			Rect tempR = Rect(i, j, size, size);
			Mat temp = labsrc(tempR);
			Mat hist;
			calcHist(&temp, 1, channels, Mat(), hist, 3, histSize, ranges, uniform, accumulate);
			float rate = compareHist(histBGcolor[count], hist, 3);
			output.at<float>(y, x) = pow(rate, mag);

			count++;
		}
	}

	return output;
}

