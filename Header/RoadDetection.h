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
Point2f A, B;

//! Normalization the matrix.
Mat Normalization(Mat src);

//! Find largest area using canny image
Mat FindLargestArea(Mat origin, Mat cannies);

/*
! Find Non edge area for road detection.
Mat src :  원본 영상(에지처리후->2진화영상으로 변환된 영상이어야함.
float sky_rate : 하늘에 해당하는 비율 (ex/ 0.3 : 상위 30%를 무시한다)
int window_size : 윈도우의 크기 : 낮을수록 정밀하게 검색.
*/
Mat nonedge_area(Mat src, float sky_rate, int window_size);

//! In Lab Color space, Filtering only L's value with sigma
Mat roadFilter(const Mat& src, double sigma, Mat mask);

//! BGR Color Space Filter with Sigma
Mat roadFilter2(const Mat& src, double sigma, Mat mask);

//! Show various color space's Scalar value on clicked pixel
void callBackFunc2(int event, int x, int y, int flags, void* userdata);

//! Find road area using similar 'LAB & BGR' in largest non edge area.
Mat FindRoad(Mat src);

int ifLRline(Point2f A, Point2f B, Point2f P);
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r);
void OpticalFlow_Count(int Pnum, vector<uchar> status, int & Car_num, Mat& frame, Point2f & pass, vector<Point2f> after, vector<Point2f> Center, Point2f A, Point2f B);

//! Calc distance between two Points.
double dist(Point2f A, Point2f B);

//! Detect and draw contour in Haar cascade's XML.
void detect_haarcascades(Mat src, string path="");//Input source & Input Cascade XML's path.

//! Distribute matrix to SEG_SIZE's matrixs and compare histogram between Origin and compare1,2,3
Mat DistHisto(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE);//Distribute Histogram Compare

/* 
! Calculate prime and secondary bin of edge direction. 1st, 2nd bin will return, not edge direction.
Origin is the source matrix, NumBins is the degree bin(divide 180 degree with NumBins).
*/
int* calcEdgeDirection(Mat Origin, int NumBins);

//! 자체 HOG
int* calcEdgeDirection2(Mat Origin, int NumBins);

//! Distribute matrix to SEG_SIZE's matrix and compare Edge direction's portion with Origin to compare 1,2,3.
Mat DistEdgeCompare(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE);

//! Using calcEdgeDirection2
Mat DistEdgeCompare2(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE);

//! Split matrix , check Edge direction.
void splitEdgeDirection(Mat Origin, int BIN_SIZE, int SEG_SIZE=0);

//! Check Direction where mouse clicked.
void callBackFunc3(int event, int x, int y, int flags, void* userdata);

//Vector Normalize.
vector<float> nomalize(vector<float> a);

//Direct area
Mat direct_area(Mat src, int window_size, float sky_rate = 0);

//For testing
bool calcEdgeDirection3(Mat Origin, int NumBins);