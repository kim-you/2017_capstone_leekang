/*
2017 이강교수님 캡스톤 팀
CCTV 영상에서 도로를 구하는 프로그램의 구현 부분
FindLargestArea -> Contour 중 가장 큰 면적을 차지하는 Contour를 구하는 프로그램.
nonedge_area -> 엣지가 없는 부분을 구해 n*n 의 mask로 씌우는 프로그램
roadFilter -> 특정한 Scalar 값을 주면 float 값의 오차범위 내에 있는 Scalar 값으로 필터링
Normalization -> 영상을 Lab 영상으로 바꾸고 밝기 값을 평활화 해주는 프로그램.
callBackFunc -> 마우스 이벤트에서 왼쪽 클릭 시, 해당하는 Color Space 표시.
*/

#include "cv.hpp" //여기에 필요한 거 다 있음
#include "opencv2/opencv.hpp" //이렇게만 하면 다 뜬다 다 뜬다
using namespace std;
using namespace cv;
Point2f A, B;

Mat Normalization(Mat src);
Mat FindLargestArea(Mat origin, Mat cannies);
Mat nonedge_area(Mat src, float sky_rate, int window_size);
Mat roadFilter(int b, int g, int r, float magnitude, const Mat& src);
Mat roadFilter2(int b, int g, int r, float magnitude, const Mat&src);
int ifLRline(Point2f A, Point2f B, Point2f P);
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r);
double dist(Point2f A, Point2f B);
void OpticalFlow_Count(int Pnum, vector<uchar> status, int & Car_num, Mat& frame, Point2f & pass, vector<Point2f> after, vector<Point2f> Center, Point2f A, Point2f B);
