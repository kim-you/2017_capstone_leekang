#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


vector<Mat> del_background_mask(vector<Mat> org, Mat flow) {

	int n = org.size();
	vector<Mat> result;
	Mat flowb = flow > 0;
	cvtColor(flowb, flowb, CV_BGR2GRAY);
	flowb.convertTo(flowb, CV_8UC1);

	for (int i = 0; i < n; i++) {
		Mat mask = org[i].clone() > 0;
		mask.convertTo(mask, CV_8UC1);

		float masksum = sum(org[i])[0] / 255;
		Mat temp = flowb & org[i];

		float andsum = sum(temp)[0] / 255;

		if (andsum / masksum < 0.2)
			continue;
		else
			result.push_back(org[i]);
	}

	return result;
}


vector<Mat> Kmeans(Mat flow, int K) {

	vector<Mat> result;

	for (int i = 0; i < K; i++) {
		Mat dstImage = Mat::zeros(flow.size(), CV_8UC1);
		result.push_back(dstImage);
	}

	int attempts = 1;
	int flags = KMEANS_RANDOM_CENTERS;
	TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 1.0);
	//kmeans 함수 매개변수 data는 1행의 입력값이 들어가야 된다. 

	//그래서 reshape 함수로 1행의 데이터 값으로 바꿔준다.
	Mat samples = flow.reshape(3, flow.rows*flow.cols);
	samples.convertTo(samples, CV_32FC3);

	Mat labels;
	kmeans(samples, K, labels, criteria, attempts, flags);

	// display clusters using labels
	for (int y = 0, k = 0; y < flow.rows; y++) {
		for (int x = 0; x < flow.cols; x++, k++)
		{
			//만약 srcImage 이미지를 1행의 행렬로 바꿔주지 않았을 시 k = y*srcImage.cols + x
			int idx = labels.at<int>(k, 0);
			//color = Vec3b(255, 255, 255);
			result[idx].at<uchar>(y, x) = 255;
		}
	}

	return result;
}


Mat segment_roabBlob(Mat roadblob, Mat segment) {

	Mat roadblob_dv = roadblob.clone();
	roadblob_dv = roadblob_dv & segment;

	return roadblob_dv;
}


Mat angle_to_HSV(Mat angleMap, int gridsize, Size resolution) {

	Mat flow = Mat::zeros(resolution, CV_8UC3); // 각도에 따른 색깔을 저장할 이미지
	for (int j = 0; j < angleMap.rows; j++) {
		for (int i = 0; i < angleMap.cols; i++) {
			const float data = angleMap.at<float>(j, i);
			float x = i * gridsize + gridsize / 2;
			float y = j * gridsize + gridsize / 2;

			if (data >= 0) {
				Scalar color = Scalar(data / 2, 127, 127);
				Rect r = Rect(x - gridsize / 2, y - gridsize / 2, gridsize, gridsize);
				rectangle(flow, r, color, -1);
			}
		}
	}
	cvtColor(flow, flow, CV_HSV2BGR);

	return flow;
}