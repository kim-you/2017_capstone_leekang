
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>
#include "point_angle_calc.h"

using namespace std;
using namespace cv;


Mat calcVec(String filename, Size resolution, int grid_size, float threshold, Mat road_Map, int check_finish, float size_ratio, float forgive_ratio, float non_ratio) {

	VideoCapture cap(filename);

	resize(road_Map, road_Map, resolution);
	int x_grids = resolution.width / grid_size;
	int y_grids = resolution.height / grid_size;

	Size scale_resol = Size(x_grids, y_grids);
	Mat flowsum_x = Mat::zeros(scale_resol, CV_32FC1);
	Mat flowsum_y = Mat::zeros(scale_resol, CV_32FC1);
	Mat flownum = Mat::zeros(scale_resol, CV_32FC1);
	Mat angle_map = Mat::zeros(scale_resol, CV_32FC1);


	int total_road_grid = 0;
	int road_grid_finish = 0;

	for (int j = 0; j < y_grids; j++) {
		for (int i = 0; i < x_grids; i++) {
			const float x = i * grid_size + grid_size / 2;
			const float y = j * grid_size + grid_size / 2;
			if (road_Map.at<uchar>(y, x) != 0)
				total_road_grid++;
		}
	} // calcultate total number of road grid

	  //////////// optical flow parameters
	double pyr_scale = 0.5, poly_sigma = 1.1;
	int pyr_levels = 3, winsize = 20, iterations = 2, poly_n = 5;
	//////////////

	bool stopflag = true; // flag which determine end of loop
	Mat img, flow, prevgray;
	int percentage = 0;
	int curpercentage = 0;

	system("cls");
	cout << "- Collecting optical flow information -" << endl << endl;
	cout << "process : " << road_grid_finish << " / " << total_road_grid << endl;
	cout << "percent : " << "0%" << "( up to " << size_ratio * 100 << "% )" << endl;

	while (stopflag) {
		cap >> img;
		resize(img, img, resolution);
		cvtColor(img, img, COLOR_BGR2GRAY);  //Grayscale img for Optical Flow

		if (prevgray.empty())
			img.copyTo(prevgray);
		//prev exist
		else {
			calcOpticalFlowFarneback(prevgray, img, flow, pyr_scale, pyr_levels, winsize, iterations, poly_n, poly_sigma, 1);

			for (int j = 0; j < y_grids; j++) {
				for (int i = 0; i < x_grids; i++) {
					if (flownum.at<float>(j, i) == check_finish)
						continue; //if a cell collect enough data, then skip this cell.
					const float x = i * grid_size + grid_size / 2;
					const float y = j * grid_size + grid_size / 2;
					const Point2f flowat = flow.at<Point2f>(y, x);
					// extract the flow data at the center of cell

					//add only if the flow is bigger than threshold
					if (sqrt(flowat.x * flowat.x + flowat.y * flowat.y) > threshold) {
						//cout << sqrt(flowat.x * flowat.x + flowat.y * flowat.y) << endl;
						flowsum_x.at<float>(j, i) += flowat.x;
						flowsum_y.at<float>(j, i) += flowat.y;
						flownum.at<float>(j, i)++;
					}

					// when number of data reach at endpoint, convert data into angle.
					if (flownum.at<float>(j, i) == check_finish) {
						angle_map.at<float>(j, i) = radtodegree_360(atan2(flowsum_y.at<float>(j, i) / flownum.at<float>(j, i), flowsum_x.at<float>(j, i) / flownum.at<float>(j, i)));
						//const Rect finishgrid(i * grid_size, j * grid_size, grid_size, grid_size);
						if (road_Map.at<uchar>(y, x) != 0) { // count whether the finished cell is road or not. 
							road_grid_finish++;
							const int current = 100 * road_grid_finish / total_road_grid;
							if (current != percentage) {
								system("cls");
								cout << "- Collecting optical flow information -" << endl << endl;
								cout << "process : " << road_grid_finish << " / " << total_road_grid << endl;
								cout << "percent : " << current << "%" << "( up to " << size_ratio * 100 << "% )" << endl;
								percentage = current;
							}
							if (((float)road_grid_finish / (float)total_road_grid) > size_ratio)
								stopflag = false;
							// if number of cells of road are filled specific ratio, finish.
						}
					}
				}
			}
			img.copyTo(prevgray);
		}
	}
	// this part determine the result of cells which did not collect enough data.
	float semi_check_finish = (float)check_finish * forgive_ratio;
	float non_finish = (float)check_finish * non_ratio;
	for (int j = 0; j < y_grids; j++) {
		for (int i = 0; i < x_grids; i++) {
			if (flownum.at<float>(j, i) > semi_check_finish) {
				angle_map.at<float>(j, i) = radtodegree_360(atan2(flowsum_y.at<float>(j, i) / flownum.at<float>(j, i), flowsum_x.at<float>(j, i) / flownum.at<float>(j, i)));
			}
			else if (flownum.at<float>(j, i) < non_finish) {
				angle_map.at<float>(j, i) = -2; // -1 means this is not road.
			}
			else
				angle_map.at<float>(j, i) = -1; // -2 means this is unknown.
		}
	}
	cap.release();
	return angle_map;
}

bool comp(const Point2f a, const Point2f b) {

	if (a.x != b.x) return a.x < b.x;
	else if (a.y != b.y) return a.y < b.y;

}

bool lineDecision(Vec4i line, int gridSize, Mat angleMap, float decision_ratio, float angle_range) {

	Point2f p1 = Point2f(line[0], line[1]);
	Point2f p2 = Point2f(line[2], line[3]);
	float angle_deg;
	float angle;
	float b;
	int xp1 = (int)p1.x / gridSize;
	int yp1 = (int)p1.y / gridSize;

	int xp2 = (int)p2.x / gridSize;
	int yp2 = (int)p2.y / gridSize;

	bool inf = false;
	if (p1.x == p2.x) {
		inf = true;
		angle_deg = 90;
	}
	else {
		angle = (p2.y - p1.y) / (p2.x - p1.x);
		b = p2.y - angle*p2.x;
		angle_deg = radtodegree(atan(angle));
	}

	vector<Point2f> sigPoints;
	sigPoints.push_back(p1);
	sigPoints.push_back(p2);

	for (int i = MIN(xp1, xp2) + 1; i <= MAX(xp1, xp2); i++)
	{
		int tempx = i * gridSize;
		int tempy = tempx * angle + b;

		sigPoints.push_back(Point2f(tempx, tempy));
	}

	for (int i = MIN(yp1, yp2) + 1; i <= MAX(yp1, yp2); i++)
	{
		int tempy;
		int tempx;
		tempy = i * gridSize;

		if (inf)
			tempx = p1.x;
		else
			tempx = (tempy - b) / angle;

		sigPoints.push_back(Point2f(tempx, tempy));
	}

	stable_sort(sigPoints.begin(), sigPoints.end(), comp);

	struct lineparts {
		int gridx;
		int gridy;
		float length;
	};

	vector<lineparts> segLine;

	for (int i = 0; i < sigPoints.size() - 1; i++) {

		lineparts temp;
		temp.length = dist_Points(sigPoints[i], sigPoints[i + 1]);
		temp.gridx = (int)sigPoints[i].x / gridSize;
		temp.gridy = (int)sigPoints[i].y / gridSize;

		segLine.push_back(temp);
	}
	float m_length = 0;

	for (int i = 0; i < segLine.size(); i++) {
		float gridangle = angleMap.at<float>(segLine[i].gridy, segLine[i].gridx);
		bool goodangle;
		if (gridangle < 0)
			goodangle = gridangle == -1 ? true : false;
		else {
			if (gridangle > 180)
				gridangle = gridangle - 180;
			goodangle = abs(gridangle - angle_deg) <= angle_range ? true : false;
		}

		if (goodangle)
			m_length += segLine[i].length;
	}

	if (m_length >= dist_Points(p1, p2)*decision_ratio)
		return true;
	else
		return false;
}


vector<Vec4i> externalLine(vector<Vec4i> lines, Size resolution, Mat roadblob) {

	Vec4f line;
	vector<Vec4i> output;
	int width = resolution.width;
	int height = resolution.height;

	float min = 1;
	float max = 0;
	int minn = 0;
	int maxn = 0;
	float ratio = 0;

	for (int n = 0; n < lines.size(); n++) {
		int p = 0;
		int q = 0;
		line = lines[n];
		float angle;
		float b;
		if (line[0] == line[2]) {
			continue;
		}
		else if (line[1] == line[3]) {
			angle = 0;
			b = line[1];
		}
		else {
			angle = (line[3] - line[1]) / (line[2] - line[0]);
			b = line[1] - angle*line[0];
		}
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				if (j >= angle*i + b && roadblob.at<uchar>(j, i) != 0)
					p++;
				else if (j < angle*i + b && roadblob.at<uchar>(j, i) != 0)
					q++;
			}
		}
		p++; q++;
		if (angle >= 0)
			ratio = (float)p / (float)q;
		else
			ratio = (float)q / (float)p;


		if (ratio >= max) {
			max = ratio;
			maxn = n;
		}

		if (ratio <= min) {
			min = ratio;
			minn = n;
		}
	}

	output.push_back(lines[maxn]);
	output.push_back(lines[minn]);

	return output;
}

vector<Vec4i> extendLine(vector<Vec4i> lines, Size resolution) {

	float width = resolution.width;
	float height = resolution.height;
	int num = lines.size();
	Vec4f line;
	for (int i = 0; i < num; i++) {
		line = lines[i];
		if (line[0] == line[2]) {
			line[1] = 0;
			line[3] = height;
		}
		else if (line[1] == line[3]) {
			line[0] = 0;
			line[2] = width;
		}
		else {
			float angle = (line[3] - line[1]) / (line[2] - line[0]);
			float b = line[1] - angle*line[0];

			Point2f ta = Point2f(-1, -1);
			Point2f tb = Point2f(-1, -1);

			float up = -b / angle;
			if (up >= 0 && up <= width) {
				Point2f temp = Point2f(up, 0);
				if (ta.x == -1)
					ta = temp;
				else
					tb = temp;
			}

			float down = (height - b) / angle;
			if (down >= 0 && down <= width) {
				Point2f temp = Point2f(down, height);
				if (ta.x == -1)
					ta = temp;
				else
					tb = temp;
			}
			float left = angle * 0 + b;
			if (left >= 0 && left <= height) {
				Point2f temp = Point2f(0, left);
				if (ta.x == -1)
					ta = temp;
				else
					tb = temp;
			}
			float right = angle * width + b;
			if (right >= 0 && right <= height) {
				Point2f temp = Point2f(width, right);
				if (ta.x == -1)
					ta = temp;
				else
					tb = temp;
			}
			line[0] = ta.x;
			line[1] = ta.y;
			line[2] = tb.x;
			line[3] = tb.y;
		}
		lines[i] = line;
	}

	return lines;
}



vector<Vec4i> findRoadLine(Mat roadBlob, Mat filtered) {

	Mat element(15, 15, CV_8U, cv::Scalar(1));
	morphologyEx(roadBlob, roadBlob, MORPH_DILATE, element);
	float w = roadBlob.cols;
	float h = roadBlob.rows;

	//모폴로지 확장 연산

	Mat canny;
	Canny(filtered, canny, 20, 40, 3);

	vector<Vec4i> lines;
	HoughLinesP(roadBlob& canny, lines, 1, CV_PI / 180, 50, 50, 20);

	return lines;
}

