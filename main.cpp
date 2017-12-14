#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>

#include "src/background_extraction.h"
#include "src/primary_road_area.h"
#include "src/line_decision.h"
#include "src/point_angle_calc.h"
#include "src/clustering.h"
#include "src/triangle_to_trapezoid.h"
#include "src/foreground_extraction.h"


using namespace std;
using namespace cv;

Size get_resolution(String videoname, float vertical);
void calcCongestion(vector<double>& congestion, int &segnum, int &trapenum, Mat &foreground, vector<vector<Rect>>& t_roi, vector<vector<Mat>>& t_roi_mask, vector<vector<int>>& t_roi_mask_sum, Rect &ROI, Mat &mask, Mat &fore_ROI);
void output(vector<double>& congestion, bool &graphic_box, bool &cmd_text, Mat &window);

int main(void) {
////////////////////////////////////////////////////////////////////PARAMETERS//////////////////////////////////////////////////////////////////

	/////Parameters for output/////
	bool graphic_box = true;
	bool cmd_text = false;
	//////////////////////////////////////

	/////Parameters for basic setting/////
	String videoname = "samplevideo/example.mp4"; //
	float vertical_size = 360;
	//////////////////////////////////////
	
	/////Parameters for background extraction/////
	int background_frame_limit_BE = 1000;  // The number of frames that determine the termination of the background extraction algorithm.
	//////////////////////////////////////

	/////Parameters for primary road area extraction
	int win_size_PRA = 5; // window size for detecting non_edge area
	float stride_PRA = 0.2; // stride ratio for detecting non_edge area
	float no_edge_rate_PRA = 0.0; // The ratio of determining no edge.
	float sky_rate_PRA = 0.3;  // The ratio of sky occupied by video (It does not have to be accurate.)
	float labcolor_limit_PRA = 10; // Sum of A and B among L, A, B color space.
	float sigma_PRA = 3; // what degree of color similarity as a road area.(sigma * standard deviation)
	float size_limit_PRA = win_size_PRA * 1.5; // Ignore too small a blob
	//////////////////////////////////////

	/////parameters for Optical flow(making the orientation map)
	int grid_size_OF = 10; // size of grids
	double vec_threshold_OF = 1; // Vector size limit not collecting data
	int finish_data_num_OF = 100; // Number of data to finish collection in one grid.
	double finish_ratio_OF = 0.8; // The criteria at which the entire data collection process ends (the size of the collected grid / the size of the road).
	double valid_ratio_OF = 0.6; // the criteria to allow a grid valid, when process finish.(In this case, there is no angle information, but it is marked as valid)
	double invalid_ratio_OF = 0.2; // the creteria to allow a grid invalid.(it is marked as invalid)
	//////////////////////////////////////

	//////Parameters for selecting lanes among the lines around the road area.
	float decision_ratio_SL = 0.7; // Matched ratio across the lines and angle map
	float decision_angle_SL = 15; // what level of angles will be recognized as a match?(degree)
	//////////////////////////////////////

	/////parameters for road clustering & segmentation
	int num_of_road_SEG = 2;  // number of road direction
	int num_of_trape_SEG = 10; // number of road partion
	float top_ratio = 0.15; // Percentage of truncation of the top when making a trapezoid in a triangle
	//////////////////////////////////////

    /////parameters for foreground detection
	int grid_size_FD = 5;  // histogram cell size
	double mag_ratio_edgeden_FD = 0.75;
	double mag_ratio_edgeangle_FD = 0.75;
	double mag_ratio_edgecolorhist_FD = 0.75;
	// Power Ratio for Increase Accuracy
	double weight_edgeden_FD = 0.3;
	double weight_edgeang_FD = 0.3;
	double weight_edgecolorhist_FD = 0.4;
	//Weight of each element
	double threshold_FD = 0.5; // Final threshold to determine foreground (0.0~1.0)
	//////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Size resolution = get_resolution(videoname, vertical_size);
		cout << "Video Name : " << videoname << endl;
		cout << "Video Size : " << resolution << endl;
		Sleep(2000);
		///// Background extraction /////
		Mat background = get_clear_background(videoname, background_frame_limit_BE, resolution);
		resize(background, background, resolution);
		/////////////////////////////////
		///// Image Filtering & Edge detection /////
		Mat bg_Bilateral;
		bilateralFilter(background, bg_Bilateral, 20, 40, 10);
		Mat bg_Canny;
		Canny(bg_Bilateral, bg_Canny, 20, 30, 3);
		/////////////////////////////////

		///// Get Primary Road Area /////
		Mat noEdgeMap = calNonedgeArea(bg_Canny, win_size_PRA, stride_PRA, no_edge_rate_PRA);
		Mat roadBlob = findRoadBlob(noEdgeMap, bg_Bilateral, sky_rate_PRA, labcolor_limit_PRA, sigma_PRA, size_limit_PRA);
	
		imshow("Primary road area", roadBlob);
		waitKey(1);
		/////////////////////////////////

		///// Get Angle Map (Optical Flow) & Clustering /////
		Mat angleMap = calcVec(videoname, resolution, grid_size_OF, vec_threshold_OF, roadBlob, finish_data_num_OF, finish_ratio_OF, valid_ratio_OF, invalid_ratio_OF);
		
		Mat angle_to_hsv = angle_to_HSV(angleMap, grid_size_OF, resolution);
		vector<Mat> result = Kmeans(angle_to_hsv, num_of_road_SEG + 1);
		vector<Mat> segment = del_background_mask(result, angle_to_hsv);
		
		
		imshow("Angle to HSV", angle_to_hsv);
		waitKey(1);
		/////////////////////////////////

		vector<vector<Rect>> t_roi;
		vector<vector<Mat>> t_roi_mask;
		vector<vector<int>> t_roi_mask_sum;
		Mat roadMask = Mat::zeros(resolution, CV_8UC1);
		Mat testmat = background.clone();

		///// Determine the outermost line of the road in each cluster /////		
		int segnum = segment.size();
		for (int i = 0; i < segnum; i++) {  // Repeat as many road clusters
			
			////// Find the line of the road and select a line that matches the angle map /////
			vector<Vec4i> lines = findRoadLine(segment[i], bg_Bilateral);
			vector<Vec4i> selectedLines;
			for (int j = 0; j < lines.size(); j++) {
				if (lineDecision(lines[j], grid_size_OF, angleMap, decision_ratio_SL, decision_angle_SL))
					selectedLines.push_back(lines[j]);
			}
			/////////////////////////////////

			////// Extend the selected line and extract the outermost line /////
			vector<Vec4i> extendLines = extendLine(selectedLines, resolution);
			Mat segroadBlob = segment_roabBlob(roadBlob, segment[i]);
			vector<Vec4i> externalLines = externalLine(extendLines, resolution, segroadBlob);
			
			
			line(testmat, Point(externalLines[0][0], externalLines[0][1]), Point(externalLines[0][2], externalLines[0][3]), Scalar(0, 0, 255));
			line(testmat, Point(externalLines[1][0], externalLines[1][1]), Point(externalLines[1][2], externalLines[1][3]), Scalar(0, 255, 0));
	
			/////////////////////////////////

			///// Divide the road into trapezoidal masks, store the mask information /////
			Vec8i line_vec8i = Vec8i(externalLines[0][0], externalLines[0][1], externalLines[0][2], externalLines[0][3], externalLines[1][0], externalLines[1][1], externalLines[1][2], externalLines[1][3]);
			vector<Vec8i> trapezoids = trapezoid_n(line_vec8i, resolution, num_of_trape_SEG, top_ratio);
		
			vector<Rect> roi;
			vector<Mat> roi_mask;
			vector<int> roi_mask_sum;

			for (int j = 0; j < num_of_trape_SEG; j++) {
				Rect r = selectROI(trapezoids[j]);
				Mat m = ROI_poly(resolution, trapezoids[j]);
				roadMask = roadMask + m;
				roi_mask.push_back(m);
				roi.push_back(r);
				roi_mask_sum.push_back(sum(m)[0] / 255);
			}
			t_roi.push_back(roi);
			t_roi_mask.push_back(roi_mask);
			t_roi_mask_sum.push_back(roi_mask_sum);

		
			for(int j = 0; j < num_of_trape_SEG; j++){
				line(testmat, Point(trapezoids[j][0], trapezoids[j][1]), Point(trapezoids[j][2], trapezoids[j][3]), Scalar(255, 0, 0));
				line(testmat, Point(trapezoids[j][4], trapezoids[j][5]), Point(trapezoids[j][6], trapezoids[j][7]), Scalar(255, 0, 0));
			}
			imshow("segmented_ROI", testmat);
			waitKey(1);
			/////////////////////////////////

		} // Closing the loop for the cluster
		
		vector<Mat> histBGcolor = bg_colorhist(grid_size_FD, background);
		vector<Mat> histBGangle = bg_edgeangle(grid_size_FD, background);
		vector<int> BGedgenum = bg_edgeden(grid_size_FD, background);
		// Background imformation for foreground extractor operation

		Mat mask = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(1, 1));
		dilate(roadMask, roadMask, mask);

		VideoCapture v(videoname);
		Mat frame;

		Size foreSize = Size(resolution.width / grid_size_FD, resolution.height / grid_size_FD);
		Mat edgeden = Mat::zeros(foreSize, CV_32FC1);
		Mat edgeang = Mat::zeros(foreSize, CV_32FC1);
		Mat colorhist = Mat::zeros(foreSize, CV_32FC1);
		Mat foreground = Mat::zeros(resolution, CV_8UC1);
		vector<double> congestion;
		while (1) {  // Loop for real-time video
			v >> frame;
			resize(frame, frame, resolution);

			///// Foreground extraction compared with background information //////
			comp_edgeden(edgeden, grid_size_FD, frame, resolution, mag_ratio_edgeden_FD, BGedgenum, roadMask, Rect(), Mat());
			comp_edgeangle(edgeang, grid_size_FD, frame, resolution, mag_ratio_edgeangle_FD, histBGangle, roadMask, Rect(), Mat(), Mat());
			comp_colorhist(colorhist, grid_size_FD, frame, resolution, mag_ratio_edgecolorhist_FD, histBGcolor, roadMask, Rect(), Mat(), Mat());
			mergeMat(foreground, resolution, edgeden, weight_edgeden_FD, edgeang, weight_edgeang_FD, colorhist, weight_edgecolorhist_FD, grid_size_FD, threshold_FD, Rect(), Mat(), Mat());
			/////////////////////////////////
			imshow("foreground", foreground);
			///// Compute the congestion and display the result. /////
			calcCongestion(congestion, segnum, num_of_trape_SEG, foreground, t_roi, t_roi_mask, t_roi_mask_sum, Rect(), Mat(), Mat());
			output(congestion, graphic_box, cmd_text, Mat());
			/////////////////////////////////
			imshow("frame", frame);
			waitKey(1);
		}

	system("PAUSE");
	return 0;
}

Size get_resolution(String videoname, float vertical) {

	Mat sampleframe;
	VideoCapture cap(videoname);
	cap >> sampleframe;
	float ratio = vertical / sampleframe.rows;
	Size resolution = Size(sampleframe.cols * ratio, vertical);
	cap.release();
	return resolution;
}


void output(vector<double>& congestion, bool &graphic_box, bool &cmd_text, Mat &window) {
	Scalar bgcolor = Scalar(0, 0, 0);
	cout << endl;
	for (int i = 0; i < congestion.size(); i++) {
		if (graphic_box) {
			bgcolor = Scalar(50, 255 * (1 - congestion[i]), 255 * congestion[i]);
			window = Mat(Size(200, 40), CV_8UC3, bgcolor);
			putText(window, to_string((int)(congestion[i] * 100)), Point(80, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255));
			imshow("[" + to_string(i + 1) + "]" + "Road", window);
		}

		if (cmd_text) {
			cout << "  Road " << (i+1) << " : " << (int)(congestion[i] * 100) << endl;
		}
	}

}

void calcCongestion(vector<double>& congestion, int &segnum, int &trapenum, Mat &foreground, vector<vector<Rect>>& t_roi, vector<vector<Mat>>& t_roi_mask, vector<vector<int>>& t_roi_mask_sum, Rect &ROI, Mat &mask, Mat &fore_ROI) {

	congestion.clear();
	double ratio = 0;
	float mask_sum = 0;
	float con_ratio = 0;
	for (int i = 0; i < segnum; i++) {
		ratio = 0;
		for (int j = 0; j < trapenum; j++) {
			ROI = t_roi[i][j];
			mask = t_roi_mask[i][j](ROI);
			mask_sum = t_roi_mask_sum[i][j];
			fore_ROI = foreground(ROI) & mask;
			ratio = ratio + ((sum(fore_ROI)[0] / 255) / mask_sum);
		}
		congestion.push_back(ratio / trapenum);
	}
}



