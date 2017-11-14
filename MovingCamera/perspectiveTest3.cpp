#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all
#include "../Header/RoadDetection_v3.h"
#include <deque>

using namespace std;
using namespace cv;


#define ROTATION 3 //Define how many rotation it is
#define FRAMERATE 30 //Define fps
#define AV_WEIGHT float(0.2)
#define DIST_SIZE 20

void callBackFunc2(int event, int x, int y, int flags, void* userdata);
vector<Vec4i> Line_sort(vector<Vec4i> raw_line);

int main(){
	int key = 0;
	int frame_rate = 30;
	Size _resize(600, 400);

	vector<Vec4i> lines;
	lines.push_back(Vec4i(-3,-7,1,3));
	lines.push_back(Vec4i(2,2,7,7));
	lines.push_back(Vec4i(-1,5,1,-5));
	lines.push_back(Vec4i(1,1,5,5));
	lines.push_back(Vec4i(-10,127,-1,-3));



	Line_sort(lines);


	Mat src = imread("../TrafficExample/Image3.jpg");

	resize(src, src, _resize);


	if (!src.data){
		printf("NO IMAGE\n");
		return -1;
	}



	imshow("ORIGIN", src);
	setMouseCallback("ORIGIN", callBackFunc2, &src); //Get Mouse Event For knowing Scalar Value.


	waitKey(0);
	//while (capture.read(src)){

	//

	//	key = waitKey(frame_rate);

	//	if (key == 27 || key == 'q')
	//		break;

	//	if (key == 32){
	//		if (frame_rate == FRAMERATE)
	//			frame_rate = 0;
	//		else if (frame_rate == 0)
	//			frame_rate = FRAMERATE;
	//	}
	//}

	return 0;

}


//Derivative -> y2 - y1 / x2 - x1
vector<Vec4i> Line_sort(vector<Vec4i> raw_line){

	for(int i = 0 ; i < raw_line.size() ; i ++){
		cout << raw_line[i] << endl;
	}

cout << endl;

vector<float> line_slope_pos;
vector<float> line_slope_neg;

vector<Vec4i> pos_lines;
vector<Vec4i> neg_lines;

float temp_slope;

	for(int i =0 ; i < raw_line.size(); i++){
		temp_slope = float(raw_line[i][1] - raw_line[i][3]) / float(raw_line[i][0] - raw_line[i][2]);

		cout << "[" << i << "] " << temp_slope << endl;

		if(temp_slope < 0){
			neg_lines.push_back(raw_line[i]);
			line_slope_neg.push_back(temp_slope);
			cout << "NEGATIVE" << endl;
		}else{
			pos_lines.push_back(raw_line[i]);
			line_slope_pos.push_back(temp_slope);
			cout << "POSITIVE" << endl;
		}

	}

	cout << "\nFirst Sort\n" << endl;

	for(int i = 0 ; i < pos_lines.size() ; i ++){
		cout << "[" << i << "] " << line_slope_pos[i] << endl;
		cout << "[" << i << "] " << pos_lines[i] << endl;
	}

	cout << endl;

	for(int i = 0 ; i < neg_lines.size() ; i ++){
		cout << "[" << i << "] " << line_slope_neg[i] << endl;
		cout << "[" << i << "] " << neg_lines[i] << endl;
	}

/*!-   Sorting Algorithm -> Bubble Sort with original array -!*/

float temp;
Vec4i temp2;

//Positive Line Sort
	for(int i = 0 ; i < line_slope_pos.size() ; i++){
		for(int j = 0 ; j < line_slope_pos.size()-(i+1); j++){
					if(line_slope_pos[j] > line_slope_pos[j+1]){
						temp = line_slope_pos[j+1];
						line_slope_pos[j+1] = line_slope_pos[j];
						line_slope_pos[j] = temp;

						temp2 = pos_lines[j+1];
						pos_lines[j+1] = pos_lines[j];
						pos_lines[j] = temp2;
					}
		}
	}

//Negative Line Sort
	for(int i = 0 ; i < line_slope_neg.size() ; i++){
		for(int j = 0 ; j < line_slope_neg.size()-(i+1); j++){
					if(line_slope_neg[j] > line_slope_neg[j+1]){
						temp = line_slope_neg[j+1];
						line_slope_neg[j+1] = line_slope_neg[j];
						line_slope_neg[j] = temp;

						temp2 = neg_lines[j+1];
						neg_lines[j+1] = neg_lines[j];
						neg_lines[j] = temp2;
					}
		}
	}

	cout << "\nSecond Sort\n" << endl;

	for(int i = 0 ; i < pos_lines.size() ; i ++){
		cout << "[" << i << "] " << line_slope_pos[i] << endl;
		cout << "[" << i << "] " << pos_lines[i] << endl;
	}

	cout << endl;

	for(int i = 0 ; i < neg_lines.size() ; i ++){
		cout << "[" << i << "] " << line_slope_neg[i] << endl;
		cout << "[" << i << "] " << neg_lines[i] << endl;
	}

	// cout << "\nAfter Sort\n" << endl;
	//
	// for(int i = 0 ; i < line_slope.size() ; i ++){
	// 	cout << "[" << i << "] " << line_slope[i] << endl;
	// 	cout << "Derv" << raw_line[i] << endl;
	// }

	return raw_line;

}



void callBackFunc2(int event, int x, int y, int flags, void* userdata){
	static int counts_number = 1;
	/* When Mouse Click, Get Scalar Values on Clicked Point */

	Mat src = *(Mat*)userdata;
	Mat src2, src3, src4, src5, src6;

	cvtColor(src, src2, CV_BGR2Lab);
	cvtColor(src, src3, CV_BGR2HSV);
	cvtColor(src, src4, CV_BGR2YCrCb);
	cvtColor(src, src5, CV_BGR2XYZ);

	switch (event){

	case EVENT_LBUTTONDOWN:

		printf("%d : X = %d, Y = %d\n", counts_number, x, y);
		printf("BGR Screen: Scalar[0] = %d, Scalar[1] = %d, Scalar[2] = %d\n",
			src.at<Vec3b>(y, x)[0], src.at<Vec3b>(y, x)[1], src.at<Vec3b>(y, x)[2]);
		printf("-----------------------------------------------------------------\n");

		rectangle(src, Point(x - 5, y - 5), Point(x + 5, y + 5), Scalar(0, 0, 255), 1, 8);

		char str[200];
		sprintf(str, "%d", counts_number);
		putText(src, str, Point(x - 7, y - 10), 1, 1, Scalar(0, 0, 255));

		imshow("ORIGIN", src);

		counts_number++;

		break;
	}

}
