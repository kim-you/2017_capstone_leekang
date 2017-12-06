/*
----------  Finding Road part  -----------

1. Normalization : Normalize the matrix (Scalar value).
2. nonedge_area : Find non edge area. Supposed non edge area was road.
3. FindLargestArea : Find the largest area in non-edge areas.
4. roadFilter : Contract LAB values for filtering from non-edge-largest area.
5. roadFilter2 : Same with roadFilter, but used BGR only.
6. callBackFunc2 : For checking scalar values from picture where clicked.
7. FindRoad : Find road area using similar 'LAB & BGR' in largest non edge area. Combined all function above.
(If you want to find road, just use this function.)

----------  Count car  -----------
(Still evaluating)

-int ifLRline(Point2f A, Point2f B, Point2f P);
-bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r);
-void OpticalFlow_Count(int Pnum, vector<uchar> status, int & Car_num, Mat& frame, Point2f & pass, vector<Point2f> after, vector<Point2f> Center, Point2f A, Point2f B);
-double dist(Point2f A, Point2f B);

----------  Machine-Learning for detect Car  -----------
(Learning is another part, this code is just for importing Haar-cascade)

1. detect_haarcascades: Input source & Input Cascade XML's path.

----------  Image Compare for rotate camera  -----------

1. DistHisto : Compare HSV histogram with distributed matrix. Not all part of Image (distribute, and compare). - Recommended -

2. calcEdgeDirection : Calculate pixel's direction, and prepared for using it for comparing images (Seems like HOG).

3. calcEdgeDirection2 : Upgraded version. Plus magnitude calculation. - Recommended - 

4. DistEdgeCompare : Compare 'Edge direction's histogram' with images. - still evaluating. maybe good for finding road-line -

5. DistEdgeCompare2 : Upgraded version. Pluse magnitude calculation.

-------- etc ---------
direct_area, nomalize, calcEdgeDirection3

-> is for testing.

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
int DistHisto(Mat Origin, Mat compare1, Mat compare2, Mat compare3, int SEG_SIZE);//Distribute Histogram Compare

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
Mat direct_area(Mat src, int window_size, float ratio, float sky_rate = 0);

//For testing
bool calcEdgeDirection3(Mat Origin, int NumBins, float _ratio = 0.8, int threshold = 10000);