
#include <opencv2/opencv.hpp>
#include <cv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat get_clear_background(String videoname, int framelimit, Size resolution) {


	VideoCapture cap(videoname);
	Ptr<BackgroundSubtractor> pMOG2;
	pMOG2 = createBackgroundSubtractorMOG2();

	int min_sum = INT_MAX;
	int current_sum = 0;
	int timer = framelimit;
	Mat img, canny, background, result;
	Mat fgMaskMOG2;
	bool stopflag = true;
	while (stopflag) {
		cap >> img;
		resize(img, img, resolution);
		pMOG2->apply(img, fgMaskMOG2);
		pMOG2->getBackgroundImage(background);
		Canny(background, canny, 30, 60);
		current_sum = sum(canny)[0] / 255;

		
		if (current_sum < min_sum) {
			min_sum = current_sum;
			result = background.clone();
			timer = framelimit;
			system("cls");
			cout << "- Background Extraction -" << endl << endl;
			cout << "Better background has been found" << endl;
			cout << "Total edge num : " << current_sum << endl;
			imshow("Background", result);
			waitKey(1);

		}
		else {
			timer--;
			if (timer % 100 == 0) {
				system("cls");
				cout << "- Background Extraction -" << endl << endl;
				cout << "Background process finishes afer " << timer << " frame." << endl;
			}
		}

		if (timer < 0)
			stopflag = false;
	}

	cap.release();
	return result;
}