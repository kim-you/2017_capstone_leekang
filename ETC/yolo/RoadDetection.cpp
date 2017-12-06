/*
2017 이강교수님 캡스톤 팀
CCTV 영상에서 도로를 구하는 프로그램의 구현 부분
FindLargestArea -> Contour 중 가장 큰 면적을 차지하는 Contour를 구하는 프로그램.
nonedge_area -> 엣지가 없는 부분을 구해 n*n 의 mask로 씌우는 프로그램
roadFilter -> 특정한 Scalar 값을 주면 float 값의 오차범위 내에 있는 Scalar 값으로 필터링
Normalization -> 영상을 Lab 영상으로 바꾸고 밝기 값을 평활화 해주는 프로그램.
*/


#include "cv.hpp" //여기에 필요한 거 다 있음
#include "opencv2/opencv.hpp" //이렇게만 하면 다 뜬다 다 뜬다

using namespace cv;
using namespace std;

int counts_number = 1;

Mat FindLargestArea(Mat origin, Mat cannies) {
	Mat src;
	Mat source;
	int i = 0, count = 0;
	int x = 0, y = 0;
	int nBlue = 0, nGreen = 0, nRed = 0;

	double maxcontour = 0;
	/*
	for (i = 0; i < 3; i++)
	bgr[i] = 0;*/

	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;


	src = origin.clone();

	findContours(cannies, contours, hierarchy, 2
		, CV_CHAIN_APPROX_SIMPLE);

	for (i = 0; i < contours.size(); i++) {
		//		printf("%d = %lf\n", i, contourArea(contours[i]));

		if (contourArea(contours[i])>maxcontour) {
			maxcontour = contourArea(contours[i]);
			count = i;
		}

	}
	source = origin.clone();
	cvtColor(source, source, CV_RGB2GRAY);
	source = Scalar(0);
	drawContours(source, contours, count, Scalar(255), CV_FILLED, 8, hierarchy);
	/*
	for (x = 0; x<origin.rows; x++){
	for (y = 0; y<origin.cols; y++){
	if (origin.at<cv::Vec3b>(y, x)[0] == 0 && origin.at<cv::Vec3b>(y, x)[1] == 0 && origin.at<cv::Vec3b>(y, x)[2] == 255){
	nBlue = src.at<cv::Vec3b>(y, x - 1)[0];
	nGreen = src.at<cv::Vec3b>(y, x - 1)[1];
	nRed = src.at<cv::Vec3b>(y, x - 1)[2];
	}
	}
	}
	bgr[0] = nBlue;
	bgr[1] = nGreen;
	bgr[2] = nRed;
	//
	printf("In Function: Blue = %d, Green = %d, Red = %d\n", bgr[0], bgr[1], bgr[2]);
	*/

	return source;
}

Mat nonedge_area(Mat src, float sky_rate, int window_size) {
	/*
	Mat src :  원본 영상(에지처리후->2진화영상으로 변환된 영상이어야함.
	float sky_rate : 하늘에 해당하는 비율 (ex/ 0.3 : 상위 30%를 무시한다)
	int window_size : 윈도우의 크기 : 낮을수록 정밀하게 검색.
	*/

	int i, i2 = 0;
	int j, j2 = 0;
	int src_height, src_width;

	src_height = src.rows;
	src_width = src.cols;

	Mat window;
	Mat output(src_height, src_width, src.type(), Scalar(0));

	float m_height = src_height * sky_rate;

	for (i = m_height; i + window_size <= src_height; i = i + window_size) {
		if (i + window_size >= src_height)
			i2 = src_height;
		else
			i2 = i + window_size;

		for (j = 0; j + window_size <= src_width; j = j + window_size) {
			if (j + window_size >= src_width)
				j2 = src_width;
			else
				j2 = j + window_size;

			window = src(Range(i, i2), Range(j, j2));
			if (sum(window) == Scalar(0))
				output(Range(i, i2), Range(j, j2)) = Scalar(255);
		}
	}
	return output;

}

Mat roadFilter(int b, int g, int r, float magnitude, const Mat&src) {
	int B, G, R;
	assert(src.type() == CV_8UC3);
	Mat filter;

	//// To
	//if ((magnitude*b)>255)   B = 255;
	//else   B = (magnitude) * b;
	//if (((magnitude)*g) > 255)   G = 255;
	//else   G = (magnitude) * g;
	//if ((magnitude*r) > 255) R = 255;
	//else   R = (magnitude) * r;

	//// From
	//b = b / magnitude;
	//g = g / magnitude;
	//r = r / magnitude;

	//mask
	inRange(src, Scalar(b - 50, g - 5, r - 10), Scalar(255, g + 5, r + 10), filter); //Threshold the image

	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));

	return filter;
}

Mat Normalization(Mat src) {

	Mat c_lab, canny;
	int low_l = 0;
	Scalar value;
	vector<Mat> lab_images(3);

	cvtColor(src, c_lab, CV_BGR2Lab);
	split(c_lab, lab_images);

	equalizeHist(lab_images[0], lab_images[0]);

	//printf("Light = %d\n", int(value.val[0]));
	merge(lab_images, c_lab);
	cvtColor(c_lab, c_lab, CV_Lab2BGR);



	return c_lab;
}

Mat roadFilter2(int b, int g, int r, float magnitude, const Mat&src) {
	int B, G, R;
	assert(src.type() == CV_8UC3);
	Mat filter;
	//mask
	inRange(src, Scalar(b - 70, g - 70, r - 70), Scalar(190, 190, 190), filter); //Threshold the image

	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	erode(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));
	dilate(filter, filter, getStructuringElement(MORPH_RECT, Size(10, 10)));

	return filter;
}

bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r)
{


	Point2f x = o2 - o1;
	Point2f d1 = p1 - o1;
	Point2f d2 = p2 - o2;

	float cross = d1.x*d2.y - d1.y*d2.x;
	if (abs(cross) < /*EPS*/1e-8)
		return false;

	double t1 = (x.x * d2.y - x.y * d2.x) / cross;
	r = o1 + d1 * t1;
	return true;
}

double dist(Point2f A, Point2f B) {
	double X_diff = 0, Y_diff = 0;
	X_diff = (A.x - B.x) * (A.x - B.x);
	Y_diff = (A.y - B.y) * (A.y - B.y);
	return sqrt(X_diff + Y_diff);
}

int ifLRline(Point2f A, Point2f B, Point2f P) {
	if (A.x != B.x) {
		double gradiant = (B.y - A.y) / (B.x - A.x);
		double D = gradiant*(P.x - A.x) + A.y;
		if (P.y > D)
			return 1;
		else if (P.y < D)
			return -1;
		else
			return 0;
	}
}

void OpticalFlow_Count(int Pnum, vector<uchar> status, int & Car_num, Mat& frame, Point2f & pass, vector<Point2f> after, vector<Point2f> Center, Point2f A, Point2f B) {
	//		calcOpticalFlowPyrLK(former_gray, latter_gray, Center, after, status, err, Size(25, 25), 3);
	for (int i = 0; i < Pnum; i++) {
		if (status[i] == 0) // if the center[i] doesn't exist at the former frame
			continue;		// continue
		else {
			circle(frame, after[i], 3, Scalar(0, 0, 255), 3, 8);	// draw estimated point
			Point2f P;
			if (intersection(Center[i], after[i], A, B, P)) {	// C - a __P__ A - B
				if ((((A.x <= P.x) && (P.x <= B.x)) || ((B.x <= P.x) && (P.x <= A.x))) && (((A.y <= P.y) && (P.y <= B.y)) || ((B.y <= P.y) && (P.y <= A.y)))) {
					//o1,p1  -- o2, p2
					if (ifLRline(A, B, after[i]) * ifLRline(A, B, Center[i]) < 0) {	//	if Center[i] is on line A-B
						if (dist(P, pass) < 4) { //	avoid double counting
							pass = Point(0, 0);	 // only once
							continue;
						}
						pass = P;	//save the point which is on the line 
						Car_num++;
						line(frame, A, B, Scalar(0, 255, 255), 10, 10, 0);
					}
				}
			}
		}
	}

}