/*
2017 이강교수님 캡스톤 팀
CCTV 영상에서 도로를 구하는 프로그램의 구현 부분
FindLargestArea -> Contour 중 가장 큰 면적을 차지하는 Contour를 구하는 프로그램.
nonedge_area -> 엣지가 없는 부분을 구해 n*n 의 mask로 씌우는 프로그램
roadFilter -> 특정한 Scalar 값을 주면 float 값의 오차범위 내에 있는 Scalar 값으로 필터링
Normalization -> 영상을 Lab 영상으로 바꾸고 밝기 값을 평활화 해주는 프로그램.
callBackFunc -> 마우스 이벤트에서 왼쪽 클릭 시, 해당하는 Color Space 표시.
LabBgrMask -> 배경화면의 Lab와 Bgr 값의 표준편차로 Mask를 씌우는 함수.
*/

#include "cv.hpp" //여기에 필요한 거 다 있음
#include "opencv2/opencv.hpp" //이렇게만 하면 다 뜬다 다 뜬다

using namespace cv;

Mat Normalization(Mat src);
Mat FindLargestArea(Mat origin, Mat cannies);
Mat nonedge_area(Mat src, float sky_rate, int window_size);
Mat roadFilter(const Mat& src, double sigma, Mat mask);
Mat roadFilter2(const Mat& src, double sigma, Mat mask);
void callBackFunc2(int event, int x, int y, int flags, void* userdata);
Mat LabBgrMask(Mat origin, Mat background);//Input: Background Matrix, Output: Mask Road Image