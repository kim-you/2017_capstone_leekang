
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


int __inline squre(int A) { return A*A; }

// Finds the intersection of two lines, or returns false.
// The lines are defined by (o1, p1) and (o2, p2).
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r) {

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

Vec8i p_lines(Vec8i line, Size winSize, float ratio) {

	Vec4i lA = { line[0], line[1], line[2], line[3] };
	Vec4i lB = { line[4], line[5], line[6], line[7] };

	Vec8i trapezoid;
	//   dy/dx


	double det = (lA[0] - lA[2])*(lB[1] - lB[3]) - (lA[1] - lA[3]) * (lB[0] - lB[2]);

	if (det == 0)
		trapezoid = { lA[0], lA[1], lA[2], lA[3], lB[0], lB[1], lB[2], lB[3] };
	else {
		double angA = atan2((lA[3] - lA[1]), (lA[2] - lA[0]));
		double angB = atan2((lB[3] - lB[1]), (lB[2] - lB[0]));
		if (angA < 0) angA = angA + CV_PI;
		if (angB < 0) angB = angB + CV_PI;
		double med_ang = (angA + angB) / (double)2;
		double diff = tan(med_ang);
		double per_diff = -1 / diff;

		Point2f intersectP;
		intersection(Point2f(lA[0], lA[1]), Point2f(lA[2], lA[3]), Point2f(lB[0], lB[1]), Point2f(lB[2], lB[3]), intersectP);
		if ((intersectP.x < 0) || (intersectP.x > winSize.width) || (intersectP.y < 0) || (intersectP.y > winSize.height)) {
			//out of window
			// no need to cut
		}
		else {
			Point2i botA, botB, newA, newB;
			if (lA[1] > intersectP.y) botA = Point2i(lA[0], lA[1]);
			else botA = Point2i(lA[2], lA[3]);
			if (lB[1] > intersectP.y) botB = Point2i(lB[0], lB[1]);
			else botB = Point2i(lB[2], lB[3]);
			newA = Point2i(botA.x + ratio * (intersectP.x - botA.x), botA.y + ratio * (intersectP.y - botA.y));
			newB = Point2i(botB.x + ratio * (intersectP.x - botB.x), botB.y + ratio * (intersectP.y - botB.y));

			trapezoid = { newA.x, newA.y, newB.x, newB.y, botA.x, botA.y, botB.x, botB.y };

		}
	}
	return trapezoid;
}


//in : two parallel lines && divide into n pieces
vector<Vec8i> trapezoid_n(Vec8i two_line, Size winSize, int n, float top_ratio) {
	vector<Vec8i> ret_rects;

	Vec8i pLines = p_lines(two_line, winSize, 1 - top_ratio);
	Vec4i lA = { pLines[0], pLines[1], pLines[2], pLines[3] };
	Vec4i lB = { pLines[4], pLines[5], pLines[6], pLines[7] };

	//sqrt ( (x2-x1)^2 + (y2-y1)^2 )
	double lengthA = sqrt(squre((lA[0] - lA[2])) + squre((lA[1] - lA[3])));
	double lengthB = sqrt(squre((lB[0] - lB[2])) + squre((lB[1] - lB[3])));

	Point2i FromA, ToA, FromB, ToB;
	double s, L;
	if (lengthA < lengthB) {
		s = lengthA;
		L = lengthB;
		FromA = Point2i(lA[0], lA[1]);
		ToA = Point2i(lB[0], lB[1]);
		FromB = Point2i(lA[2], lA[3]);
		ToB = Point2i(lB[2], lB[3]);
	}
	else {
		s = lengthB;
		L = lengthA;
		FromA = Point2i(lB[0], lB[1]);
		ToA = Point2i(lA[0], lA[1]);
		FromB = Point2i(lB[2], lB[3]);
		ToB = Point2i(lA[2], lA[3]);
	}
	double r = pow((L / s), 1 / (double)n);
	double YA = sqrt(squre((lA[0] - lB[0])) + squre((lA[1] - lB[1])));
	double YB = sqrt(squre((lA[2] - lB[2])) + squre((lA[3] - lB[3])));
	Point2f directionA = Point2f((ToA.x - FromA.x) / YA, (ToA.y - FromA.y) / YA);
	Point2f directionB = Point2f((ToB.x - FromB.x) / YB, (ToB.y - FromB.y) / YB);
	Point2f tempA(FromA.x, FromA.y), tempB(FromB.x, FromB.y);

	double first_tirmA = YA * (r - 1) / (pow(r, n) - 1);
	double first_tirmB = YB * (r - 1) / (pow(r, n) - 1);

	vector<Point2i> ret_points;

	ret_points.push_back(FromA);
	ret_points.push_back(FromB);

	for (int i = 0; i < n - 1; i++) {
		double an = first_tirmA * pow(r, i);   // an = a * r ^ (n-1)
		double bn = first_tirmB * pow(r, i);
		tempA = Point2f(tempA.x + an * directionA.x, tempA.y + an * directionA.y);
		tempB = Point2f(tempB.x + bn * directionB.x, tempB.y + bn * directionB.y);
		ret_points.push_back(tempA);
		ret_points.push_back(tempB);
	}

	ret_points.push_back(ToA);
	ret_points.push_back(ToB);

	Vec8i tmp;
	for (int i = 0; i < n; i++) {
		tmp = { ret_points[2 * i].x, ret_points[2 * i].y, ret_points[2 * i + 1].x, ret_points[2 * i + 1].y,
			ret_points[2 * i + 2].x, ret_points[2 * i + 2].y, ret_points[2 * i + 3].x, ret_points[2 * i + 3].y };
		ret_rects.push_back(tmp);
	}

	return ret_rects;
}

Mat ROI_poly(Size resolution, Vec8i pt) {

	Point pts[4];
	pts[0] = Point(pt[0], pt[1]);
	pts[3] = Point(pt[2], pt[3]);
	pts[1] = Point(pt[4], pt[5]);
	pts[2] = Point(pt[6], pt[7]);

	Mat mask = Mat::zeros(resolution, CV_8UC1);
	fillConvexPoly(mask, pts, 4, Scalar(255));

	return mask;
}



Rect selectROI(Vec8i pts) {

	int x[4] = { pts[0], pts[2], pts[4], pts[6] };
	int y[4] = { pts[1], pts[3], pts[5], pts[7] };

	int max_x = 0;
	int max_y = 0;
	int min_x = INT_MAX;
	int min_y = INT_MAX;

	for (int i = 0; i < 4; i++) {
		if (x[i] > max_x)
			max_x = x[i];

		if (x[i] < min_x)
			min_x = x[i];
	}

	for (int i = 0; i < 4; i++) {
		if (y[i] > max_y)
			max_y = y[i];

		if (y[i] < min_y)
			min_y = y[i];
	}

	Rect r = Rect(Point(min_x, min_y), Point(max_x, max_y));
	return r;
}