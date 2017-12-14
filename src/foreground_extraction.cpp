
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat neighb_filter(Mat &src, int ksize, double weight, int &winsize) {

	Mat dst;
	Mat kernel = Mat::ones(ksize, ksize, CV_32F) / ((float)(ksize*ksize) - 1);
	kernel.at<float>(2, 2) = 0;

	filter2D(src, dst, -1, kernel);

	dst = dst*0.3 + src;
	morphologyEx(dst, dst, MORPH_DILATE, Mat());

	return dst;
}

void mergeMat(Mat &output, Size &resolution, Mat &den, double &denw, Mat &angle, double &anglew, Mat &color, double &colorw, int &winsize, double &threshold, Rect &r, Mat &total, Mat &filter) {

	//Mat total(Size(den.cols, den.rows), CV_32FC1);
	//addWeighted(den, denw, angle, anglew, 0, total);
	total = den * denw + angle *anglew + color*colorw;
	//Mat result = neighb_filter(total, 3, 1, winsize);
	filter = neighb_filter(total, 3, 1, winsize);
	Scalar score = Scalar(0, 0, 0);
	int rows = filter.rows;
	int cols = filter.cols;
	for (int j = 0; j < rows; j++) {
		for (int i = 0; i < cols; i++)
		{
			float rate = filter.at<float>(j, i);
			r = Rect(i*winsize, j*winsize, winsize, winsize);
			score = Scalar(rate * 255, 0, 0);
			rectangle(output, r, score, -1);
		}
	}
	output = output > 255 * threshold;
}

vector<Mat> bg_edgeangle(int &size, Mat &src) {

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


void comp_edgeangle(Mat &output, int &size, Mat &src, Size &resolution, double &mag, vector<Mat>& histBGangle, Mat &roadMask, Rect &r, Mat &temp, Mat &hist) {


	Mat gray, sobel_x, sobel_y;
	cvtColor(src, gray, CV_BGR2GRAY);

	Sobel(gray, sobel_x, CV_32FC1, 1, 0);
	Sobel(gray, sobel_y, CV_32FC1, 0, 1);

	Mat Mag(gray.size(), CV_32FC1);
	Mat Angle(gray.size(), CV_32FC1);
	Mat edge_angle(gray.size(), CV_32FC1);

	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);

	float tempangle = 0;
	float tempmag = 0;
	for (int i = 0; i < Angle.rows; i++) {
		for (int j = 0; j < Angle.cols; j++) {
			tempmag = Mag.at<float>(i, j);
			tempangle = Angle.at<float>(i, j);

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
	int count = 0;
	int x, y = 0;
	double rate = 0;

	for (int j = 0; j + size <= resolution.height; j += size, y++) {
		x = 0;
		for (int i = 0; i + size <= resolution.width; i += size, x++) {
			if (roadMask.at<uchar>(j + size / 2, i + size / 2) == 0) {
				output.at<float>(y, x) = 0;
				count++;
				continue;
			}
			r = Rect(i, j, size, size);
			temp = edge_angle(r);
			calcHist(&temp, 1, channels, Mat(), hist, 1, histSize, ranges);
			rate = compareHist(histBGangle[count], hist, 3);
			output.at<float>(y, x) = pow(rate, 1);
			count++;
		}
	}
}


vector<int> bg_edgeden(int &size, Mat &src) {

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


void comp_edgeden(Mat &output, int &size, Mat &src, Size &resolution, double &mag, vector<int>& BGedgenum, Mat &roadMask, Rect &r, Mat &temp) {


	Mat canny;
	double pixnum = size * size;
	Canny(src, canny, 60, 80);
	int count = 0;
	int x, y = 0;
	int edgenum = 0;
	double ratio = 0;
	for (int j = 0; j + size <= resolution.height; j += size, y++) {
		x = 0;
		for (int i = 0; i + size <= resolution.width; i += size, x++) {
			if (roadMask.at<uchar>(j + size / 2, i + size / 2) == 0) {
				output.at<float>(y, x) = 0;
				count++;
				continue;
			}
			r = Rect(i, j, size, size);
			temp = canny(r);
			edgenum = sum(temp)[0] / 255;
			ratio = (double)(abs(BGedgenum[count] - edgenum) / pixnum);
			//Scalar Color = Scalar(0,0,pow(255 *ratio,1.2));
			output.at<float>(y, x) = pow(ratio, mag);

			count++;
		}
	}
}



vector<Mat> bg_colorhist(int &size, Mat &src) {


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


void comp_colorhist(Mat &output, int &size, Mat &src, Size &resolution, double &mag, vector<Mat>& histBGcolor, Mat &roadMask, Rect &r, Mat &temp, Mat &hist) {

	Mat labsrc;
	cvtColor(src, labsrc, CV_BGR2Lab);

	int histSize[] = { 3, 5, 5 };
	float Lranges[] = { 0, 255 };
	float abranges[] = { 0, 255 };
	const float* ranges[] = { Lranges, abranges, abranges };
	const int channels[] = { 0, 1, 2 };
	int count = 0;
	int x, y = 0;
	double rate = 0;
	for (int j = 0; j + size <= resolution.height; j += size, y++) {
		x = 0;
		for (int i = 0; i + size <= resolution.width; i += size, x++) {
			if (roadMask.at<uchar>(j + size / 2, i + size / 2) == 0) {
				output.at<float>(y, x) = 0;
				count++;
				continue;
			}
			r = Rect(i, j, size, size);
			temp = labsrc(r);
			calcHist(&temp, 1, channels, Mat(), hist, 3, histSize, ranges);
			rate = compareHist(histBGcolor[count], hist, 3);
			output.at<float>(y, x) = pow(rate, mag);
			count++;
		}
	}
}

