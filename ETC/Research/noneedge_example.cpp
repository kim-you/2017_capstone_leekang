#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>
#include <time.h> 



using namespace std;
using namespace cv;

Mat nonedge_area(Mat src, float sky_rate, int window_size) {
	/*
	Mat src :  원본 영상(에지처리후->2진화영상으로 변환된 영상이어야함.
	float sky_rate : 하늘에 해당하는 비율 (ex/ 0.3 : 상위 30%를 무시한다)
	int window_size : 검색윈도우의 크기 : 낮을수록 정밀하게 검색. 짝수 기준으로 계산 / 홀수넣을시 넣은값 -1 / 최소값 4(그이하들어오면 4로 전환) 
	*/

	int i, i2 = 0;
	int j, j2 = 0;
	int src_height, src_width;
	src_height = src.rows;
	src_width = src.cols;

	if (window_size % 2 == 1)
		window_size--;

	if (window_size < 4)
		window_size = 4;
	//window size 값 보정.

	Mat window; 
	Mat output(src_height, src_width, src.type(), Scalar(0));

	int m_height = (int)(src_height * sky_rate); // 하늘 비율을 고려한 새로운 총 높이.

	for (i = m_height+2; i + window_size <= src_height-2; i = i + window_size) {
		if (i + window_size >= src_height)
			continue; //윈도우 사이즈 이하의 여백이 남으면 건너뜀.
		else
			i2 = i + window_size;

		for (j = 2; j + window_size <= src_width-2; j = j + window_size) {
			if (j + window_size >= src_width)
				continue;
				//j2 = src_width;
			else
				j2 = j + window_size;

			window = src(Range(i, i2), Range(j, j2)); //윈도우 영역을 저장

			if (sum(window) == Scalar(0)) {  // 윈도우 영역에 엣지가 없다면?
				output(Range(i, i2), Range(j, j2)) = Scalar(255);
				
			//	circle(output, k, (window_size/2), Scalar(255), -1);
			//	circle(output, k, 2, Scalar(0));
				
			}
		}
	}
	return output;

}


Mat find_road(Mat src, Mat backimg, float lablimit, int ffmode, int sizelimit, float sky_rate, float sigma) {
	//src : nonedge_area 함수의 결과물인 마스크 이미지
	//backimg : 배경 이미지
	//lablimit : lab에서 ab의 제한값
	//fflimit : floodfill의 인접 상하한선
	//ffmode : floodfill의 모드. 4 혹은 8
	
	
	int largest_area = 0;
	int largest_contour_index = 0;

	Mat temp(src.rows, src.cols, src.type());

	Mat srcclone = src.clone();
	Mat backclonebgr = backimg.clone();
	Mat backclonelab = backimg.clone();

	cvtColor(backclonelab, backclonelab, CV_BGR2Lab);
	
	//erode(srcclone, srcclone, Mat());

	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(srcclone, contours, hierarchy, RETR_EXTERNAL, 2);


//	findContours(srcclone, contours, hierarchy, RETR_CCOMP, 2);

	vector<Moments> mu(contours.size());
	vector<Point2f> mc(contours.size());
	vector<Scalar> meanbgr(contours.size());
	vector<Scalar> meanlab(contours.size());
	vector<Scalar> stddevbgr(contours.size());
	vector<Scalar> stddevlab(contours.size());
	vector<float> contourarea(contours.size());
	vector<Point> result(contours.size());
	
	float m_height = src.rows * sky_rate;
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i]);
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

		contourarea[i] = contourArea(contours[i]);
		if (contourarea[i] > largest_area && mc[i].y > m_height) {
			largest_area = contourarea[i];
			largest_contour_index = i;
		}
		temp = Scalar(0);
		drawContours(temp, contours, i, Scalar(255), -1, 8, hierarchy);
		meanStdDev(backclonebgr, meanbgr[i], stddevbgr[i], temp);
		meanStdDev(backclonelab, meanlab[i], stddevlab[i], temp);
		//drawContours(backclonebgr, contours, i, mean[i], -1, 8, hierarchy);
	}

	Mat test = src.clone();
	test = Scalar(0);

	drawContours(test, contours, largest_contour_index, Scalar(255), -1, 8, hierarchy);
	imshow("4. Largest Blob", test);


	test = src.clone();
	for (int i = 0; i < contours.size(); i++) {

		circle(test, mc[i], 2, Scalar(80), -1, 8, 0);
	}
	imshow("5. every mass center point", test);


	Scalar std_colorlab = meanlab[largest_contour_index];
	Scalar std_colorbgr = meanbgr[largest_contour_index];

	float stdl = std_colorlab[0];
	float devl = stddevlab[largest_contour_index][0] * sigma;
	
	for (int i = 0; i < mc.size();) {
		
		float diff = abs(std_colorlab[1] - meanlab[i][1]) + abs(std_colorlab[2] - meanlab[i][2]);
		if ((diff > lablimit) || (contourarea[i] < sizelimit*sizelimit) || (meanlab[i][0]<(stdl-devl)) || (meanlab[i][0] > (stdl+devl))) {
			mu.erase(mu.begin() + i);
			mc.erase(mc.begin() + i);
			meanbgr.erase(meanbgr.begin() + i);
			meanlab.erase(meanlab.begin() + i);
			stddevbgr.erase(stddevbgr.begin() + i);
			stddevlab.erase(stddevlab.begin() + i);
			contours.erase(contours.begin() + i);
			contourarea.erase(contourarea.begin() + i);
		}

		else {
			i++;
		}

	}


	for (int i = 0; i < contours.size(); i++) {
		circle(test, mc[i], 2, Scalar(100), 3, 8, 0);
	}
	imshow("6. selected points", test);


	Rect boundRect;
	Mat backclonegray;
	cvtColor(backclonebgr, backclonegray , CV_BGR2GRAY);
	Canny(backclonegray, backclonegray, 35, 50, 3);
	
	int size = 3;
	Mat elements(size, size, CV_8U, cv::Scalar(1));

	morphologyEx(backclonegray, backclonegray, MORPH_CLOSE, elements);
	
	/*
	dilate(backclonebgr, backclonebgr, Mat());

	erode(backclonebgr, backclonebgr, Mat());
	
	*/
	imshow("7. After Morph Canny", backclonegray);

	cvtColor(backclonegray, backclonegray, CV_GRAY2BGR);

	float q = 3;
		for (int i = 0; i < mc.size(); i++) {
		/*
		stddev[i][0] = stddev[i][0] * q;
		stddev[i][1] = stddev[i][1] * q;
		stddev[i][2] = stddev[i][2] * q;*/
		//	if (backclonegray.at<uchar>(mc[i]) == 0) {
				floodFill(backclonegray, mc[i], Scalar(150, 150, 0));
				circle(backclonegray, mc[i], 2, Scalar(255, 0, 255), -1, 8, 0);
			
			//floodFill(backclonebgr, mc[i], Scalar(255, 255, 0), &boundRect, Scalar::all(5), Scalar::all(5), ffmode);
		//circle(backclonebgr, mc[i], 7, meanbgr[i], -1, 8, 0);
		floodFill(backclonebgr, mc[i], Scalar(255, 255, 0), &boundRect, Scalar::all(2), Scalar::all(2), 4);
		circle(backclonebgr, mc[i], 2, Scalar(255, 0, 255), -1, 8, 0);

	}

		imshow("8. at Canny", backclonegray);

		imshow("9. at BGR", backclonebgr);
	
		return srcclone;
	//return backclonebgr;
}

int main(void) {
	Mat frame, poly, color_mask;
	Mat test, test1;
	Mat back, fore, canny, gray;
	//VideoCapture cap("example.avi");
	//VideoCapture cap("traffic3.mp4");
	int start = 0;
	vector<Mat> channels(3);

	while (1) {
		frame = imread("20.jpg");
		resize(frame, frame, Size(480, 360));
		imshow("original", frame);
		
		if (!frame.data) {
			cout << "end" << endl;
			break;
		}

		Mat filtered;
	//GaussianBlur(frame, filtered, Size(3, 3), 0.1);
		//blur(frame, filtered, Size(3, 3));
		bilateralFilter(frame, filtered, 25, 30, 50);
		//medianBlur(filtered, filtered, 5);
		imshow("1. filtered", filtered);


		
		cvtColor(filtered, gray, CV_BGR2GRAY);

		Canny(gray, canny, 20 , 30, 3);
		imshow("2. canny", canny);
		threshold(canny, canny, 127, 255, CV_THRESH_BINARY);

		int poly_size = 6;
		float sky_rate = 0;
		poly = nonedge_area(canny, sky_rate, poly_size);

		imshow("3. poly", poly);

		float lab_limit = 15;
		int ff_mode = 4;
		Mat test3;
		test3 = find_road(poly, filtered, lab_limit, ff_mode, poly_size, 0.3, 3);

	//	imshow("result", test1);

		//		imshow("Frame", test);

		//imshow("1", channels[0]);
		//imshow("2", channels[1]);
		//imshow("3", channels[2]);
		//imshow("Canny", canny);
		//imshow("Background", frame);
		

		if (waitKey(1) >= 0) break;
	}
	return 0;
}

