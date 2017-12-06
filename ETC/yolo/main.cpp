#include <iostream>
#include <iomanip> 
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>

#define OPENCV

#include "yolo_v2_class.hpp"	// imported functions from DLL

#include <opencv2/opencv.hpp>			// C++
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>

using namespace std;
using namespace cv;
//using namespace cuda;

static void drawArrows(Mat frame, const vector<Point2f> prevPts, const vector<Point2f> nextPts, const vector<uchar> status, Scalar line_color = Scalar(255, 0, 255))
{
	for (size_t i = 0; i < prevPts.size(); ++i)
	{
		if (status[i])
		{
			int line_thickness = 3;

			Point p = prevPts[i];
			Point q = nextPts[i];

			double angle = atan2((double)p.y - q.y, (double)p.x - q.x);

			double hypotenuse = sqrt((double)(p.y - q.y)*(p.y - q.y) + (double)(p.x - q.x)*(p.x - q.x));

			if (hypotenuse < 1.0)
				continue;

			// Here we lengthen the arrow by a factor of three.
			q.x = (int)(p.x - 3 * hypotenuse * cos(angle));
			q.y = (int)(p.y - 3 * hypotenuse * sin(angle));

			// Now we draw the main line of the arrow.
			line(frame, p, q, line_color, line_thickness);

			// Now draw the tips of the arrow. I do some scaling so that the
			// tips look proportional to the main line of the arrow.

			p.x = (int)(q.x + 9 * cos(angle + CV_PI / 4));
			p.y = (int)(q.y + 9 * sin(angle + CV_PI / 4));
			line(frame, p, q, line_color, line_thickness);

			p.x = (int)(q.x + 9 * cos(angle - CV_PI / 4));
			p.y = (int)(q.y + 9 * sin(angle - CV_PI / 4));
			line(frame, p, q, line_color, line_thickness);
		}
	}
}

void get_Pts(vector<bbox_t> result_vec, vector<Point2f> &center_pts) {
	center_pts.clear();
	for (int i= 0; i<result_vec.size();i++)
		center_pts.push_back(Point2f(result_vec[i].x + result_vec[i].w*0.5, result_vec[i].y + result_vec[i].h*0.5));
}

void draw_boxes(Mat mat_img, vector<bbox_t> result_vec, vector<string> obj_names, vector<Point2f> prev_pts, vector<Point2f> next_pts, vector<uchar> status, unsigned int wait_msec = 0) {
	drawArrows(mat_img, prev_pts, next_pts, status, Scalar(255, 0, 0));

	for (auto &i : result_vec) {
		Scalar color;

		if (i.obj_id == 0) // car
			color = Scalar(160, 60, 60);
		else
			color = Scalar(60, 160, 160);
		rectangle(mat_img, Rect(i.x, i.y, i.w, i.h), color, 5);
		if (obj_names.size() > i.obj_id) {
			string obj_name = obj_names[i.obj_id];
			if (i.track_id > 0) obj_name += " - " + to_string(i.track_id);
			Size const text_size = getTextSize(obj_name, FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
			int const max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
			rectangle(mat_img, Point2f(max((int)i.x - 3, 0), max((int)i.y - 30, 0)),
				Point2f(min((int)i.x + max_width, mat_img.cols - 1), min((int)i.y, mat_img.rows - 1)), color, CV_FILLED, 8, 0);
			putText(mat_img, obj_name, Point2f(i.x, i.y - 10), FONT_HERSHEY_COMPLEX_SMALL, 1.2, Scalar(0, 0, 0), 2);
		}
	}
	imshow("window name", mat_img);
	waitKey(wait_msec);
}

void show_result(vector<bbox_t> const result_vec, vector<string> const obj_names) {
	for (auto &i : result_vec) {
		if (obj_names.size() > i.obj_id) cout << obj_names[i.obj_id] << " - ";
		cout << "obj_id = " << i.obj_id << ",  x = " << i.x << ", y = " << i.y
			<< ", w = " << i.w << ", h = " << i.h
			<< setprecision(3) << ", prob = " << i.prob << endl;
	}
}

vector<string> objects_names_from_file(string const filename) {
	ifstream file(filename);
	vector<string> file_lines;
	if (!file.is_open()) return file_lines;
	for (string line; file >> line;) file_lines.push_back(line);
	cout << "object names loaded \n";
	return file_lines;
}

int main(int argc, char *argv[])
{

//	Detector detector("yolo-voc.cfg", "yolo-voc.weights");
//	auto obj_names = objects_names_from_file("data/voc.names");
	
	Detector detector("yolo-obj.cfg", "8.23_5k.weights");
	auto obj_names = objects_names_from_file("data/obj.names");


	while (true)
	{
		string filename;
		//		cout << "input image or video filename: ";
		//		cin >> filename;
		filename = "D:\\CV\\sample34.mp4";
		if (filename.size() == 0) break;

		try {
			Mat frame, prev_frame, det_frame, g_prev, gray;
			vector<bbox_t> result_vec, thread_result_vec;
			vector<Point2f> prev_pts, next_pts;

			detector.nms = 0.02;	// comment it - if track_id is not required
			thread td([]() {});
			atomic<int> ready_flag;
			ready_flag = false;
			VideoCapture cap(filename);
			TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);

			int cnt = 0;
			for (; cap >> frame, cap.isOpened();) {
				cvtColor(frame, gray, COLOR_BGR2GRAY);
				get_Pts(result_vec, prev_pts);
				td.join();
				result_vec = thread_result_vec;
				det_frame = frame;
				td = thread([&]() { thread_result_vec = detector.detect(det_frame, 0.20); ready_flag = true; });
				if (!prev_frame.empty()) {
					vector<uchar> status;
					vector<float> err;
					if(prev_pts.size()>0)
						calcOpticalFlowPyrLK(g_prev, gray, prev_pts, next_pts, status, err, Size(25, 25), 3, termcrit, 0, 0.001);
					draw_boxes(prev_frame, result_vec, obj_names, prev_pts, next_pts, status, 3);
	//				show_result(result_vec, obj_names);
				}
				prev_frame = frame;
				gray.copyTo(g_prev);
				ready_flag = false;
			}
		}
		catch (exception &e) { cerr << "exception: " << e.what() << "\n"; getchar(); }
		catch (...) { cerr << "unknown exception \n"; getchar(); }
		filename.clear();
	}

	return 0;
}
