/*
2017 �̰������� ĸ���� ��
CCTV ���󿡼� ���θ� ���ϴ� ���α׷��� ���� �κ�
FindLargestArea -> Contour �� ���� ū ������ �����ϴ� Contour�� ���ϴ� ���α׷�.
nonedge_area -> ������ ���� �κ��� ���� n*n �� mask�� ����� ���α׷�
roadFilter -> Ư���� Scalar ���� �ָ� float ���� �������� ���� �ִ� Scalar ������ ���͸�
Normalization -> ������ Lab �������� �ٲٰ� ��� ���� ��Ȱȭ ���ִ� ���α׷�.
callBackFunc -> ���콺 �̺�Ʈ���� ���� Ŭ�� ��, �ش��ϴ� Color Space ǥ��.
*/

#include "cv.hpp" //���⿡ �ʿ��� �� �� ����
#include "opencv2/opencv.hpp" //�̷��Ը� �ϸ� �� ��� �� ���
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
