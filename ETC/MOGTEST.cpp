#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all
#include "RoadDetection.h"
#include <deque>

using namespace std;
using namespace cv;


#define ROTATION 3 //Define how many rotation it is
#define FRAMERATE 30 //Define fps
#define AV_WEIGHT float(0.2)
#define DIST_SIZE 20

int number = 0;



int main(){
	Size _resize(600, 400);
	bool turn_flag = true;

	int queue_sum = 0;
	float average_w = 0;

	deque<int> average_queue;

	int* min = new int(3);

	min[0] = 1000000;
	min[1] = 1000000;
	min[2] = 1000000;
	int KNN_count = 0;
	int flag = -1;
	int average_edge = 0;
	int key, frame_rate = FRAMERATE;

	int camera_flag = 0;

	VideoCapture capture("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic.mp4");
	//VideoCapture capture(0);

	Mat src, gray, mogtest, mogtest2, knn_canny;
	vector<Mat> Dist_KNN;
	vector<int> Dist_int;
	vector<int> Minimum;
	vector<Mat> Origin_KNN;

	Mat Merge_KNN(_resize, CV_8UC3);

	capture >> src;

	//Ptr<BackgroundSubtractorMOG2> vc[ROTATION];

	//for (int i = 0; i < ROTATION; i++){
	//	vc[i] = createBackgroundSubtractorMOG2();
	//}
	Ptr<BackgroundSubtractorMOG2> vc[ROTATION];
	Ptr<BackgroundSubtractorKNN> vc2;
	Ptr<BackgroundSubtractorKNN> vc3;

	vc[0] = createBackgroundSubtractorMOG2();
	vc[1] = createBackgroundSubtractorMOG2();
	vc[2] = createBackgroundSubtractorMOG2();
	vc2 = createBackgroundSubtractorKNN();
	vc3 = createBackgroundSubtractorKNN();

	Mat mogtest3, mogtest4;
	int a;
	Mat Temp_mog;
	int min_mog_size = 1000000;

	if (!src.data){
		printf("NO IMAGE\n");
		cin >> a;
		return -1;
	}
	Temp_mog = src.clone();
	while (capture.read(src)){

		resize(src, src, _resize); //Resize for better quality of view.
	
		vc2->apply(src, mogtest3);
		vc2->getBackgroundImage(mogtest4);
	

		//medianBlur(mogtest4, mogtest4, 3);

		imshow("ORIGINAL", src);
		//imshow("MOGTEST", mogtest);
		//imshow("MOG2", mogtest2);
		imshow("KNN", mogtest4);
		Canny(mogtest4, knn_canny, 30, 50);
		//Canny(mogtest4, knn_canny, 50, 100);
		cout << countNonZero(knn_canny) << endl;
		if (countNonZero(knn_canny) <= min_mog_size && countNonZero(knn_canny) >=10){
			min_mog_size = countNonZero(knn_canny);
			Temp_mog = mogtest4.clone();
			cout << "UPDATED=====================================" << endl;
		}
		
		imshow("TEMPMOG", Temp_mog);
		
		for (int i = 0; i < src.rows; i += DIST_SIZE){
			for (int j = 0; j < src.cols; j += DIST_SIZE){
				Rect temp_rect(j, i, DIST_SIZE, DIST_SIZE);
				Mat temp_image(knn_canny, Rect(temp_rect));
				Mat temp_image2(mogtest4, Rect(temp_rect));
				//cout << "COUNTNONZERO" << countNonZero(temp_image) << endl;
				Dist_KNN.push_back(temp_image);
				Dist_int.push_back(countNonZero(temp_image));
				Origin_KNN.push_back(temp_image2);

				if (camera_flag < 3){
					Minimum.push_back(1000000);
				}
			}	
		}

		//cout << Dist_int.size() << endl;
		//cout << Dist_KNN.size() << endl;

		//imshow("TEST_DIST", Dist_KNN[0]);

		if (turn_flag == true){
			for (int i = 0; i < src.rows; i += DIST_SIZE){
				for (int j = 0; j < src.cols; j += DIST_SIZE){
					//cout << "MINIMUM " <<Minimum[KNN_count] << endl;
					//Dist_KNN[KNN_count].copyTo(Merge_KNN(Rect(j, i, DIST_SIZE, DIST_SIZE)));
					Origin_KNN[KNN_count].copyTo(Merge_KNN(Rect(j, i, DIST_SIZE, DIST_SIZE)));
					KNN_count++;
				}
			}
		}
		else{
			for (int i = 0; i < src.rows; i += DIST_SIZE){
				for (int j = 0; j < src.cols; j += DIST_SIZE){
					//cout << "MINIMUM " << Minimum[KNN_count] << endl;
					if (Dist_int[KNN_count] <= Minimum[KNN_count]){
						//Dist_KNN[KNN_count].copyTo(Merge_KNN(Rect(j, i, DIST_SIZE, DIST_SIZE)));
						Origin_KNN[KNN_count].copyTo(Merge_KNN(Rect(j, i, DIST_SIZE, DIST_SIZE)));
						Minimum[KNN_count] = Dist_int[KNN_count];
					}
					KNN_count++;
				}
			}		
		}
		
		imshow("RESULT", Merge_KNN);
	
		key = waitKey(frame_rate);

		if (key == 27 || key == 'q')
			break;

		if (key == 32){
			if (frame_rate == FRAMERATE)
				frame_rate = 0;
			else if (frame_rate == 0)
				frame_rate = FRAMERATE;
		}
		Dist_KNN.clear();
		Dist_int.clear();
		Minimum.clear();
		KNN_count = 0;
		turn_flag = false;
		camera_flag++;
	
	}
	
	return 0;

}