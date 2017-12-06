#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>
#include <time.h> 

using namespace std;
using namespace cv;
Mat angle_roabBlob(Mat roadblob, Mat angleMap, int wsize);
vector<Vec4i> externalLine(vector<Vec4i> lines, Size resolution, Mat roadblob, Mat angleMap);
vector<Vec4i> extendLine(vector<Vec4i> lines, Size resolution);
Mat calNonedgeArea(Mat, int , float , float);
Mat calDirectionMap(Mat, int, int, float, float, float); // 수정예정
bool calDirection(Mat, int, float, float); // 수정예정
Mat findRoadBlob(Mat, Mat, float, float, float, float);
float dist_Points(Point2f, Point2f);
float radtodegree(float);
bool lineDecision(Vec4i line, int gridSize, Mat angleMap, float decision_ratio, float angle_range);
bool comp(const Point2f a, const Point2f b);
vector<Vec4i> findRoadLine(Mat, Mat, float, float);
vector<Vec4i> findRoadLine2(Mat, Mat, float, float);
Mat calcVec(String filename, Size resolution, int grid_size, float threshold, Mat road_Map, int check_finish, float size_ratio, float forgive_ratio, float non_ratio);

int main(void) {

		int num = 2;
		
		Mat frame;
	
	//resize(frame, frame, Size(frame.cols*zoom, frame.rows*zoom));

	//float zoom = 0.5;
	//VideoCapture cap("example.avi");
	//VideoCapture cap("traffic3.mp4");
	while (1) {
		frame = imread("C:\\Users\\Administrator\\Documents\\Visual Studio 2013\\Projects\\OPENCVP3\\OPENCVP3\\Background_k.jpg");
	
		string filename = to_string(num) + ".jpg";
		
		float ratio = 360.0 / frame.rows;
		Size resolution = Size(frame.cols * ratio, 360);
		resize(frame, frame, resolution);
		
	//// 세로길이 360에 맞추고 가로길이는 가변으로 리사이징

	//	imshow("original", frame);

		if (!frame.data) {
			cout << "end" << endl;
			break;
		}
		
		Mat filtered;
		bilateralFilter(frame, filtered, 20, 40, 10);
	//양방향 필터링
	//	imshow("1. filtered", filtered);
	
		Mat canny;
		Canny(filtered, canny, 20, 30, 3);
		//imshow("2. canny", canny);

		int win_size_nedge = 7; //윈도우 크기
		float stride_nedge = 0.2; //윈도우 이동간격
		float edgerate = 0.0; //edge가 있더라도 허용하는 비율
		Mat noEdgeMap;
		noEdgeMap = calNonedgeArea(canny, win_size_nedge, stride_nedge, edgerate);
		//imshow("3. NoedgeMap", noEdgeMap);
	//캐니 앳지 영상에서 엣지가 없는 부분을 추출.

		Mat roadBlob;
		float sky_rate = 0.3;  // 하늘이 차지하는 비율
		float lab_limit = 10; // AB 색상 제한 수치
		float sigma = 2; //분산 적용 수치
		float size_limit = win_size_nedge * 1.5; // 블롭 사이즈 제한
		roadBlob = findRoadBlob(noEdgeMap, filtered, sky_rate, lab_limit, sigma, size_limit);
		//imshow("5. RoadBlob", roadBlob);
		//엣지가 없는 부분중 도로영역을 구분.

		float skipAngle = 10; // 같다고 보는 각도
		float skipDist = 20; // 같다고 보는 거리(화면의 가장자리 부분)

		Mat angleMap = calcVec("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic.mp4", resolution, 5, 0.5, roadBlob, 50, 0.7, 0.7, 0.15);


		Mat flow = Mat::zeros(resolution, CV_32FC3);


			for (int j = 0; j < angleMap.rows; j++) {
				for (int i = 0; i < angleMap.cols; i++) {
					const float data = angleMap.at<float>(j, i);
					float x = i * 5 + 5 / 2;
					float y = j * 5 + 5 / 2;

					if (data >= 0)
						line(flow, Point(x, y), Point(x + 3 * cos(data*CV_PI / 180), y + 3 * sin(data*CV_PI / 180)), Scalar(255, 0, 0), 1);

					else if (data == -1)
						//Rect r = Rect(Point(j * 5 , i * 5), Point(j * 5, i * 5));
						circle(flow, Point(x, y), 1, Scalar(0, 1, 0), -1);

					else if (data == -2)
						circle(flow, Point(x, y), 1, Scalar(0, 0, 1), -1);


				}
			}

			imshow("fl;s", flow);

		
		cout << "Angle Map completed." << endl;
		vector<Vec4i> lines = findRoadLine2(roadBlob, filtered, 10, 20);

		cout << "Lines completed. " << endl;

		vector<Vec4i> selectedLines;

		//for (int i = 0; i < lines.size(); i++){
		//	if (lineDecision(lines[i], 10, angleMap, 70))
		//		selectedLines.push_back(lines[i]);
		//}
		for (int i = 0; i < lines.size(); i++){
			Vec4i l = lines[i];
			if (lineDecision(lines[i], 5, angleMap, 0.7, 5)) {
				//line(filtered, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 1, CV_AA);
				selectedLines.push_back(lines[i]);
			}
			else
				line(filtered, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
		}

		vector<Vec4i> extend = extendLine(selectedLines, resolution);

		for (int i = 0; i < extend.size(); i++) {
			line(filtered, Point(extend[i][0], extend[i][1]), Point(extend[i][2], extend[i][3]), Scalar(0, 255, 0), 1, CV_AA);
		}
		Mat agroadBlob = angle_roabBlob(roadBlob, angleMap, 5);

		imshow("safs", agroadBlob);
		vector<Vec4i> external = externalLine(extend, resolution, agroadBlob, angleMap);

			line(filtered, Point(external[0][0], external[0][1]), Point(external[0][2], external[0][3]), Scalar(255, 0, 0), 1, CV_AA);
			line(filtered, Point(external[1][0], external[1][1]), Point(external[1][2], external[1][3]), Scalar(0, 0, 255), 1, CV_AA);



		//cout << "Lines Selected. " << endl;
		//cout << lines.size() << "   " << selectedLines.size();

		//for (size_t i = 0; i < selectedLines.size(); i++)
		//{
		//	Vec4i l = selectedLines[i];
		//	line(filtered, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
		//}

		namedWindow("result");
		moveWindow("result", 400, 400);
		imshow("sfsa", roadBlob);
		imshow("result", filtered);

		waitKey();
		num++;
		system("PAUSE");
	}

	return 0;
}

Mat angle_roabBlob(Mat roadblob, Mat angleMap, int wsize) {

	for (int j = 0; j < angleMap.rows; j++) {
		for (int i = 0; i < angleMap.cols; i++) {
			const float data = angleMap.at<float>(j, i);

			if (data == -2) {
				float x = i * wsize;
				float y = j * wsize;
				Rect r = Rect(Point(x, y), Point(x + wsize, y + wsize));
				rectangle(roadblob, r, Scalar(0, 0, 0), -1);
			}
		}
	}
	return roadblob;
}
vector<Vec4i> externalLine(vector<Vec4i> lines, Size resolution, Mat roadblob, Mat angleMap) {

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
		else if (line[1] == line[3]){
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

		p++;
		q++;

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
		cout << max << " " << min << endl;
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

			float up = - b / angle;
			if (up >= 0 && up <= width) {
				Point2f temp = Point2f(up, 0);
				if (ta.x == -1)
					ta = temp;
				else
					tb = temp;
			}

			float down = (height - b)/angle;
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


float radtodegree(float n) {
	float temp = n*(180.0 / CV_PI);

	if (temp < 0)
		return 180 - abs(temp);
	else
		return temp;
}

vector<Vec4i> findRoadLine2(Mat roadBlob, Mat filtered, float sameAngle, float samedist) {


	Mat element(15, 15, CV_8U, cv::Scalar(1));
	morphologyEx(roadBlob, roadBlob, MORPH_DILATE, element);
	float w = roadBlob.cols;
	float h = roadBlob.rows;

	//모폴로지 확장 연산

	Mat canny;
	Canny(filtered, canny, 40, 60, 3);

	vector<Vec4i> lines;
	struct lineInfo {
		Vec4i line;
		float angle;
		vector<Point2f> points;
	};

	vector<lineInfo> selected;

	HoughLinesP(roadBlob& canny, lines, 1, CV_PI / 180, 40, 5, 30);

	Point2f a;

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];

		lineInfo temp;
		temp.line = lines[i];
		temp.angle = radtodegree(atan2(l[3] - l[1], l[2] - l[0]));
		float a = (float)(l[3] - l[1]) / (float)(l[2] - l[0]); //y = ax + b
		float b = l[1] - a * l[0];

		float y1 = b;
		if (y1 >= 0 && y1 <= h) temp.points.push_back(Point2f(0, y1));
		float y2 = a*w + b;
		if (y2 >= 0 && y2 <= h) temp.points.push_back(Point2f(w, y2));

		float x1 = -(b / a);
		if (x1 >= 0 && x1 <= w) temp.points.push_back(Point2f(x1, 0));

		float x2 = (h - b) / a;
		if (x2 >= 0 && x2 <= w) temp.points.push_back(Point2f(x2, h));

		bool flag = false;
		for (int j = 0; j < selected.size(); j++)
		{
			if (temp.angle > selected[j].angle - sameAngle && temp.angle < selected[j].angle + sameAngle) {
				float a = dist_Points(temp.points[0], selected[j].points[0]);
				float c = dist_Points(temp.points[1], selected[j].points[0]);
				if (a < samedist || c < samedist)
					flag = true;
				else {
					float b = dist_Points(temp.points[0], selected[j].points[1]);
					float d = dist_Points(temp.points[1], selected[j].points[1]);
					if (b < samedist || d < samedist)
						flag = true;
				}
			}

		}

		if (flag == false)
			selected.push_back(temp);

	}
	

	lines.clear();
	for (int i = 0; i < selected.size(); i++)
	{

		lines.push_back(selected[i].line);

	}

	return lines;


}


vector<Vec4i> findRoadLine(Mat roadBlob, Mat filtered, float sameAngle, float samedist) {


	Mat element(15, 15, CV_8U, cv::Scalar(1));
	morphologyEx(roadBlob, roadBlob, MORPH_DILATE, element);

	//모폴로지 확장 연산
	vector<Vec4i> lines;
	Mat canny;
	Canny(filtered, canny, 40, 60, 3);

	HoughLinesP(roadBlob& canny, lines, 1, CV_PI / 180, 40, 10, 30);

	float w = roadBlob.cols;
	float h = roadBlob.rows;

	return lines;

}


float radianTodegree(float angle) {
	
	float d_angle = angle * 57.2958;
	
	if (d_angle < 0) 
		return d_angle = -d_angle;
		
	else 
		return 180 - d_angle;
}

Mat calNonedgeArea(Mat src, int windowSize, float stride, float edgeRate) {
	/*
	Mat src :  원본 영상(에지처리후->2진화영상으로 변환된 영상이어야함.
	float sky_rate : 하늘에 해당하는 비율 (ex/ 0.3 : 상위 30%를 무시한다)
	int window_size : 검색윈도우의 크기 : 낮을수록 정밀하게 검색.
	float stride : 검색 윈도우의 이동 간격(1 = 윈도우사이즈만큼 / 0.5 = 윈도우사이즈의 반만큼 이동)
	*/

	float i, j;
	int height = src.rows;
	int	width = src.cols;

	Mat window;
	Mat output(height, width, src.type(), Scalar(0));

	int wstride = MAX(windowSize * stride, 1);

	for (i = 0; i + windowSize <= height; i += wstride) {

		for (j = 0; j + windowSize <= width; j += wstride) {

			window = src(Range(i, i + windowSize), Range(j, j + windowSize)); //윈도우 영역을 저장
		
			if (sum(window)[0] / 255<= windowSize * windowSize * edgeRate)   // 윈도우 영역 내에 엣지가 정한 비율보다 낮다면
				output(Range(i, i + windowSize), Range(j, j + windowSize)) = Scalar(255);
		}
	}

	return output;

}



Mat calDirectionMap(Mat src, int windowSize, int numBin, float mininumEdge, float ratio, float stride) {
	/*
	Mat src :  원본 영상(에지처리후->2진화영상으로 변환된 영상이어야함.
	float sky_rate : 하늘에 해당하는 비율 (ex/ 0.3 : 상위 30%를 무시한다)
	int window_size : 윈도우의 크기 : 낮을수록 정밀하게 검색.
	*/

	float i, j;
	int height = src.rows;
	int	width = src.cols;

	Mat window;
	Mat output(height, width, src.type(), Scalar(0));

	int wstride = MAX(windowSize * stride, 1);

	for (i = 0; i + windowSize <= height; i += wstride) {

		for (j = 0; j + windowSize <= width; j += wstride) {

			window = src(Range(i, i + windowSize), Range(j, j + windowSize));

			if (calDirection(window, numBin, mininumEdge, ratio)) {
				output(Range(i, i+windowSize), Range(j, j + windowSize)) += Scalar(50, 50, 50);
			}
			else {
				output(Range(i, i + windowSize), Range(j, j + windowSize)) -= Scalar(50, 50, 50);
			}
		}
	}

	return output;

}


float dist_Points(Point2f a, Point2f b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

bool calDirection(Mat window, int NumBins, float minedge, float ratio) {

	Mat src = window.clone();
	Mat output;
	vector<float> bin_hist(NumBins, 0);

	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	
	Mat sobel_x, sobel_y;
	Sobel(gray, sobel_x, CV_32FC1, 1, 0, -1);
	Sobel(gray, sobel_y, CV_32FC1, 0, 1, -1);

	Mat Mag(gray.size(), CV_32FC1);
	Mat Angle(gray.size(), CV_32FC1);

	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);

	float tAngle, leftover, rate, tMag;
	int binnum;
	int bin_degree = 180 / NumBins;
	for (int i = 0; i < Angle.rows; i++) {

		for (int j = 0; j < Angle.cols; j++) {
			
			tAngle = Angle.at<float>(i, j);
			tAngle = (tAngle >= 180) ? tAngle - 180 : tAngle;

			binnum = tAngle / bin_degree;
			leftover = tAngle - (binnum * bin_degree);
			rate = leftover / bin_degree;
			tMag = Mag.at<float>(i, j);
			
			bin_hist[binnum] += tMag * (1 - rate);
			if ((binnum + 1) * bin_degree >= 180)
				bin_hist[0] += tMag * rate;
			else 
				bin_hist[binnum + 1] += tMag * rate;
		}
	}

	float sum, temp = 0;
	int max_bin;
	for (int i = 0; i < NumBins; i++) {
		if (bin_hist[i] >= temp) {
			max_bin = i;
			temp = bin_hist[i];
		}
		sum += bin_hist[i];
	}


	if (sum <= minedge)
		return false;

	vector<float> bin_average(NumBins, 0);

	for (int i = 0; i < NumBins; i++)
		bin_average[i] = bin_hist[i] / sum;

	int temp_bin = (max_bin == 0) ? NumBins : max_bin;
	if (bin_average[max_bin] + MAX(bin_average[temp_bin - 1], bin_average[(max_bin + 1) % NumBins]) >= ratio)
		return true;
	else
		return false;

}


Mat findRoadBlob(Mat nonEdgeMap, Mat filtered, float sky_rate, float labLimit, float sigma, float sizeLimit) {

	float largestArea = 0;
	int largestIndex;

	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	

	Mat temp = nonEdgeMap.clone();
	erode(temp, temp, Mat());

	//findContours(temp, contours, hierarchy, RETR_EXTERNAL, 2);
	findContours(temp, contours, hierarchy, 2, 2);

	float m_height = nonEdgeMap.rows * sky_rate;

	vector<float> contourarea(contours.size());
	vector<Moments> mu(contours.size());
	vector<Point2f> mc(contours.size());


	for (int i = 0; i < contours.size(); i++)
	{
		contourarea[i] = contourArea(contours[i]);
		mu[i] = moments(contours[i]);
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

		if (contourarea[i] > largestArea && mc[i].y > m_height) {
			largestArea = contourarea[i];
			largestIndex = i;
		}
	}

	temp = Scalar(0);
	drawContours(temp, contours, largestIndex, Scalar(255), -1, 8, hierarchy);
	
	Mat src_Lab;
	cvtColor(filtered, src_Lab, CV_BGR2Lab);
	Scalar stdmean, stddev;

	meanStdDev(src_Lab, stdmean, stddev, temp);

	float minSize = sizeLimit * sizeLimit;
	float L_limit = 0;
	
	Mat output = nonEdgeMap.clone();
	output = Scalar(0);

	for (int i = 0; i < contours.size(); i++) {

		if (contourarea[i] < minSize)
			continue;

		temp = Scalar(0);
		Scalar tempstdmean, tempstddev;
		drawContours(temp, contours, i, Scalar(255), -1, 8, hierarchy);
		meanStdDev(src_Lab, tempstdmean, tempstddev, temp);
		float diff = abs(stdmean[1] - tempstdmean[1]) + abs(stdmean[2] - tempstdmean[2]);
		//waitKey(30);
		//Mat god;
	//	filtered.copyTo(temp2, temp);
	/*	system("cls");
		cout << "유효한 L값 : " << stdmean[0] - L_limit << " ~ " << stdmean[0] + L_limit << "\n";
		cout << "현재 블롭 L값 : " << tempstdmean[0] << "\n";
		cout << "기준 블롭과 AB값 차이 : " << diff << "\n";*/
				/*
				imshow("fff", temp2);
				waitKey(30);
				system("PAUSE");*/

		//L_limit = stddev[0] * sigma;
		if (diff < 1) L_limit = L_limit * 2;
		else L_limit = stddev[0] * sigma;

		if ((diff < labLimit) && tempstdmean[0] > (stdmean[0] - L_limit) && tempstdmean[0] < (stdmean[0] + L_limit))
			output += temp;
	}
	//cout << contours.size() << "\n";
	return output;
}

///*
//
//	Mat srcHSV;
//	cvtColor(filtered, srcHSV, COLOR_BGR2Lab);
//	//cvtColor(srcHSV, srcHSV, COLOR_Lab2BGR);
//	int channels[] = { 1, 2 };
//	int bin0 = 256, bin1 = 256;
//	int histSize[] = { bin0, bin1 };
//	float range0[] = { 0, 256 };
//	float range1[] = { 0, 256 };
//	MatND histBase, histCompare;
//	const float* ranges[] = { range0, range1 };
//	calcHist(&srcHSV, 1, channels, temp, histBase, 2, histSize, ranges, true, false);
//	normalize(histBase, histBase, 0, 1, NORM_MINMAX, -1, Mat());
//
//	Mat output = nonEdgeMap.clone();
//	output = Scalar(0);
//
//	for (int i = 0; i < contours.size(); i++)
//	{
//		temp = Scalar(0);
//		drawContours(temp, contours, i, Scalar(255), -1, 8, hierarchy);
//		calcHist(&srcHSV, 1, channels, temp, histCompare, 2, histSize, ranges, true, false);
//		normalize(histCompare, histCompare, 0, 1, NORM_MINMAX, -1, Mat());
//		float result = compareHist(histBase, histCompare, 0);
//		cout << (float)result << "\n";
//		imshow("fff", temp);
//		waitKey(30);
//		system("PAUSE");
//		if (result >0.3)
//			output += temp;
//	}
//	return output;
//}
//Mat find_road(Mat nonedgeMap, Mat filtered, Mat backimg, Mat direct_mask, float lablimit, int ffmode, int sizelimit, float sky_rate, float sigma) {
//	//src : nonedge_area 함수의 결과물인 마스크 이미지
//	//backimg : 배경 이미지
//	//lablimit : lab에서 ab의 제한값
//	//fflimit : floodfill의 인접 상하한선
//	//ffmode : floodfill의 모드. 4 혹은 8
//
//
//	int largestArea = 0;
//	int largestIndex = 0;
//
//	Mat temp = nonedgeMap.clone();
//	Mat srcclone = nonedgeMap.clone();
//	
//	float m_height = nonedgeMap.rows * sky_rate;
//
//	Mat backclonebgr = backimg.clone();
//	Mat backclonelab = backimg.clone();
//
//	cvtColor(backclonelab, backclonelab, CV_BGR2Lab);
//
//	erode(srcclone, srcclone, Mat());
//
//	vector< vector<Point> > contours;
//	vector<Vec4i> hierarchy;
//
//	findContours(srcclone, contours, hierarchy, RETR_EXTERNAL, 2);
//	//findContours(srcclone, contours, hierarchy, RETR_TREE, 2);
//
//
//	vector<Moments> mu(contours.size());
//	vector<Point2f> mc(contours.size());
//	vector<Scalar> meanbgr(contours.size());
//	vector<Scalar> meanlab(contours.size());
//	vector<Scalar> stddevbgr(contours.size());
//	vector<Scalar> stddevlab(contours.size());
//	vector<float> contourarea(contours.size());
//	vector<Point> result(contours.size());
//
//	for (int i = 0; i < contours.size(); i++)
//	{
//		contourarea[i] = contourArea(contours[i]);
//		mu[i] = moments(contours[i]);
//		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
//
//		if (contourarea[i] > largestArea && mc[i].y > m_height) {
//			largestArea = contourarea[i];
//			largestIndex = i;
//		}
//
//	}
//	temp = Scalar(0);
//	drawContours(temp, contours, largestIndex, Scalar(255), -1, 8, hierarchy);
//	
//	
//	meanStdDev(backclonebgr, meanbgr[i], stddevbgr[i], temp);
//	meanStdDev(backclonelab, meanlab[i], stddevlab[i], temp);
//		//drawContours(backclonebgr, contours, i, mean[i], -1, 8, hierarchy);
//	}
//
//	Mat test = src.clone();
//	test = Scalar(0);
//
//	drawContours(test, contours, largest_contour_index, Scalar(255), -1, 8, hierarchy);
//	Mat test_1;
//	origin.copyTo(test_1, test);
//	Mat test_gray;
//	cvtColor(test_1, test_gray, CV_BGR2GRAY);
//
//	Mat sobel_x, sobel_y;
//	Sobel(test_gray, sobel_x, CV_32FC1, 1, 0, -1);
//	Sobel(test_gray, sobel_y, CV_32FC1, 0, 1, -1);
//
//	Mat Mag(test_gray.size(), CV_32FC1);
//	Mat Angle(test_gray.size(), CV_32FC1);
//
//
//	cartToPolar(sobel_y, sobel_x, Mag, Angle, true);
//	cout << contourarea[largest_contour_index] << " " << (float)sum(Mag)[0] / contourarea[largest_contour_index] << "\n";
//	imshow("asdf", sobel_x);
//	imshow("asdfs", sobel_y);
//
//	//Mat test_1 = origin.copyTo(test);
//	imshow("4. Largest Blob", test_1);
//
//
//	test = src.clone();
//	for (int i = 0; i < contours.size(); i++) {
//
//		circle(test, mc[i], 2, Scalar(80), -1, 8, 0);
//	}
//	imshow("5. every mass center point", test);
//
//
//	Scalar std_colorlab = meanlab[largest_contour_index];
//	Scalar std_colorbgr = meanbgr[largest_contour_index];
//
//	float stdl = std_colorlab[0];
//	float devl = stddevlab[largest_contour_index][0] * sigma;
//
//	Mat temp1 = src.clone();
//	Mat team2;
//
//	for (int i = 0; i < mc.size(); i++) {
//
//		float diff = abs(std_colorlab[1] - meanlab[i][1]) + abs(std_colorlab[2] - meanlab[i][2]);
//
//		if ((diff > lablimit) || (contourarea[i] < sizelimit*sizelimit) || (meanlab[i][0]<(stdl - devl)) || (meanlab[i][0] > (stdl + devl))) {
//
//			mu.erase(mu.begin() + i);
//			mc.erase(mc.begin() + i);
//			hierarchy.erase(hierarchy.begin() + i);
//			meanbgr.erase(meanbgr.begin() + i);
//			meanlab.erase(meanlab.begin() + i);
//			stddevbgr.erase(stddevbgr.begin() + i);
//			stddevlab.erase(stddevlab.begin() + i);
//			contours.erase(contours.begin() + i);
//			contourarea.erase(contourarea.begin() + i);
//		}
//
//		else {
//			i++;
//		}
//
//	}
//
//	test = Scalar(0);
//
//	for (int i = 0; i < contours.size(); i++) {
//		drawContours(test, contours, i, Scalar(255), -1, 8, hierarchy);
//	}
//
//	imshow("6. selected points", test);
//
//
//	Rect boundRect;
//	Mat backclonegray;
//	cvtColor(backclonebgr, backclonegray, CV_BGR2GRAY);
//	Canny(backclonegray, backclonegray, 35, 50, 3);
//
//	int size = 3;
//	Mat elements(size, size, CV_8U, cv::Scalar(1));
//
//	morphologyEx(backclonegray, backclonegray, MORPH_CLOSE, elements);
//
//	/*
//	dilate(backclonebgr, backclonebgr, Mat());
//	erode(backclonebgr, backclonebgr, Mat());
//
//	
//	imshow("7. After Morph Canny", backclonegray);
//
//	cvtColor(backclonegray, backclonegray, CV_GRAY2BGR);
//	Mat testmask = imread("testmask.jpg", IMREAD_GRAYSCALE);
//
//	float q = 3;
//	for (int i = 0; i < mc.size(); i++) {
//		/*
//		stddev[i][0] = stddev[i][0] * q;
//		stddev[i][1] = stddev[i][1] * q;
//		stddev[i][2] = stddev[i][2] * q;*/
//		//	if (backclonegray.at<uchar>(mc[i]) == 0) {
//		floodFill(backclonegray, mc[i], Scalar(150, 150, 0));
//		circle(backclonegray, mc[i], 2, Scalar(255, 0, 255), -1, 8, 0);
//
//		//floodFill(backclonebgr, mc[i], Scalar(255, 255, 0), &boundRect, Scalar::all(5), Scalar::all(5), ffmode);
//		//circle(backclonebgr, mc[i], 7, meanbgr[i], -1, 8, 0);
//		floodFill(backclonebgr, mc[i], Scalar(255, 255, 0), &boundRect, Scalar::all(3), Scalar::all(3), 4);
//
//		//floodFill(backclonebgr, testmask, mc[i], Scalar(255, 255, 0), &boundRect, Scalar::all(2), Scalar::all(2), 4);
//		circle(backclonebgr, mc[i], 2, Scalar(255, 0, 255), -1, 8, 0);
//
//	}
//
//	imshow("8. at Canny", backclonegray);
//
//	imshow("9. at BGR", backclonebgr);
//
//	return srcclone;
//	//return backclonebgr;
//}
//*/




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

	cout << "process : " << road_grid_finish << " / " << total_road_grid << endl;
	cout << "percent : " << road_grid_finish / total_road_grid * 100 << "%" << endl;

	bool stopflag = true; // flag which determine end of loop
	Mat img, flow, prevgray;
	int percentage = 0;
	int curpercentage = 0;
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
						angle_map.at<float>(j, i) = radtodegree(atan2(flowsum_y.at<float>(j, i) / flownum.at<float>(j, i), flowsum_x.at<float>(j, i) / flownum.at<float>(j, i)));
						//const Rect finishgrid(i * grid_size, j * grid_size, grid_size, grid_size);
						if (road_Map.at<uchar>(y, x) != 0) { // count whether the finished cell is road or not. 
							road_grid_finish++;
							const int current = 100 * road_grid_finish / total_road_grid;
							if (current != percentage) {
								system("cls");
								cout << "process : " << road_grid_finish << " / " << total_road_grid << endl;
								cout << "percent : " << current << "%" << "( up to " << size_ratio*100 <<"% )"<< endl;
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
				angle_map.at<float>(j, i) = radtodegree(atan2(flowsum_y.at<float>(j, i) / flownum.at<float>(j, i), flowsum_x.at<float>(j, i) / flownum.at<float>(j, i)));
				//cout << radtodegree(atan2(flowsum_y.at<float>(j, i) / flownum.at<float>(j, i), flowsum_x.at<float>(j, i) / flownum.at<float>(j, i))) << " stored at " << j << "," << i << endl;
			}
			else if (flownum.at<float>(j, i) < non_finish) {
				angle_map.at<float>(j, i) = -2; // -2 means this is not road.
			}
			else
				angle_map.at<float>(j, i) = -1; // -1 means this is unknown.
		}
	}
	return angle_map;
}



bool comp(const Point2f a, const Point2f b) {

	if (a.x != b.x) return a.x < b.x;
	else if (a.y != b.y) return a.y < b.y;

}
float distPoints(Point2f a, Point2f b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y)*(a.y - b.y));
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


	//for (int i = 0; i < sigPoints.size(); i++) {
	//	cout << sigPoints[i] << endl;
	//}

	stable_sort(sigPoints.begin(), sigPoints.end(), comp);

	struct lineparts {
		int gridx;
		int gridy;
		float length;
	};

	vector<lineparts> segLine;

	for (int i = 0; i < sigPoints.size() - 1; i++) {

		lineparts temp;
		temp.length = distPoints(sigPoints[i], sigPoints[i + 1]);
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
			//float negangle = gridangle < 90 ? gridangle + 90 : gridangle - 90;
			//goodangle = abs(gridangle - angle_deg) <= abs(negangle - angle_deg) ? true : false;
			goodangle = abs(gridangle - angle_deg) <= angle_range ? true : false;
		}

		if (goodangle)
			m_length += segLine[i].length;
		//	cout << sigPoints[i] << "  " << segLine[i].gridy << " " << segLine[i].gridx << "  " << segLine[i].length << endl;
	}


	if (m_length >= distPoints(p1, p2)*decision_ratio)
		return true;
	else
		return false;
}
