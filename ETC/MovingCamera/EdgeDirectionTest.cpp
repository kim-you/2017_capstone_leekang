#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all
#include "RoadDetection.h"

using namespace std;
using namespace cv;

#define NumBins int(6)

int main()
{
	Mat src = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\ROtation1_1.jpg", 1);
	Mat src2 = imread("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\ROtation2_1.jpg", 1);
	Mat gray, sobel_x, sobel_y, result, canny;
	
	int bin_degree = 180 / NumBins;
	
	vector<int> bin_hist(NumBins);


	float tan_val = 0;
	int test = 0;
	int count = 1;
	
	resize(src, src, Size(400, 300));
	resize(src2, src2, Size(400, 300));

	cvtColor(src, gray, CV_BGR2GRAY);
	
	
	Sobel(gray, sobel_x, CV_8UC1, 1, 0, 3);
	convertScaleAbs(sobel_x, sobel_x);

	Sobel(gray, sobel_y, CV_8UC1, 0, 1, 3);
	convertScaleAbs(sobel_y, sobel_y);

	addWeighted(sobel_x, 0.5, sobel_y, 0.5, 0, result);
	result = result > 64;

	cout << "Sobel(x) " << countNonZero(sobel_x) << endl;
	cout << "Sobel(y) " << countNonZero(sobel_y) << endl;
	cout << "Sobel(result) " << countNonZero(result) << endl;
	cout << "===========================================" << endl;
	
	for (int i = 0; i < src.rows; i++){
		for (int j = 0; j < src.cols; j++){
			//printf("[%d][%d] = %f\n", j, i, tan_val);
			if (result.at<uchar>(i, j) == 255){
				//rectangle(src, Point(j - 5, i - 5), Point(j + 5, i + 5), Scalar(0, 0, 255), 1);
				tan_val = fastAtan2(i, j-(src.cols/2));
				//printf("[%d][%d] = %f\n", j, i, tan_val);

				for (int z = 0; z < NumBins; z++){
					if (z*bin_degree <= int(tan_val) && int(tan_val) < (z + 1)*bin_degree){
						bin_hist[z]++;
					}
					
				}
			}
		}
	
	}

	for (int i = 0; i < NumBins; i++){
		printf("bin[%d][%d] = %d\n", i*bin_degree, (i + 1)*bin_degree, bin_hist[i]);
		bin_hist[i] = 0;
	}

	//for (int i = 0; i < src.rows; i++){
	//	for (int j = 0; j < src.cols; j++){
	//	
	//		//printf("[%d][%d] = %f\n", j, i, fastAtan2(src.rows - ((src.rows / 2) - i), j - src.cols / 2));
	//		
	//		//printf("REAL [%d][%d]\n", j - (src.cols / 2), -((src.rows - i) - (src.rows / 2)));
	//		//printf("D : %f\n===================================================\n", fastAtan2(-((src.rows - i) - (src.rows / 2)), j - (src.cols / 2)));
	//		
	//		if (result.at<Vec3b>(i, j) == Vec3b(255,255,255)){
	//			test++;
	//			tan_val = fastAtan2(-((src.rows - i) - (src.rows / 2)), j - (src.cols / 2));
	//			/*printf("CHECK\n");
	//			printf("REAL [%d][%d]\n", j - (src.cols / 2), -((src.rows - i) - (src.rows / 2)));
	//			printf("D : %f\n===================================================\n", tan_val);*/

	//			for (int z = 1; z <= NumBins; z++){
	//				if ((z - 1)*bin_degree <= tan_val && tan_val < z*bin_degree)
	//					bin_hist[z-1]++;
	//			}
	//		}
	//	}
	//}

	cout << "===========================END============================" << endl;




	cvtColor(src2, gray, CV_BGR2GRAY);


	Sobel(gray, sobel_x, CV_8UC1, 1, 0, 3);
	convertScaleAbs(sobel_x, sobel_x);

	Sobel(gray, sobel_y, CV_8UC1, 0, 1, 3);
	convertScaleAbs(sobel_y, sobel_y);

	addWeighted(sobel_x, 0.5, sobel_y, 0.5, 0, result);
	result = result > 64;


	for (int i = 0; i < src.rows; i++){
		for (int j = 0; j < src.cols; j++){
			//printf("[%d][%d] = %f\n", j, i, tan_val);
			if (result.at<uchar>(i, j) == 255){
				//rectangle(src, Point(j - 5, i - 5), Point(j + 5, i + 5), Scalar(0, 0, 255), 1);
				tan_val = fastAtan2(i, j - (src.cols / 2));
				//printf("[%d][%d] = %f\n", j, i, tan_val);

				for (int z = 0; z < NumBins; z++){
					if (z*bin_degree <= int(tan_val) && int(tan_val) < (z + 1)*bin_degree){
						bin_hist[z]++;
					}

				}
			}
		}

	}

	for (int i = 0; i < NumBins; i++)
		printf("bin[%d][%d] = %d\n", i*bin_degree, (i + 1)*bin_degree, bin_hist[i]);

























	imshow("ORIGIN", src);
	imshow("ORIGIN2", src2);
	//imshow("X", sobel_x);
	//imshow("y", sobel_y);
	//imshow("TT", result);
	waitKey(0);
	
		return 0;

	}