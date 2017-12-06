#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all

using namespace std;
using namespace cv;

int main(){
	int i = 0, count = 0, n_count=0;
	int positive_key = 0, negative_key = 0;
	int key, frame_rate = 30;
	VideoCapture capture = ("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic4.mp4");
	Mat src, det, gray, hough;
	Mat origin,canny;
	
	float positive=20, negative=-20;
	float temp_x1[100], temp_x2[100], temp_y1[100], temp_y2[100];
	float x1[100], x2[100], y1[100], y2[100], slide[100];
	float n_x1[100], n_x2[100], n_y1[100], n_y2[100], n_slide[100];


	Vec4i slope;

	Mat morph;
	Mat kernel = getStructuringElement(0, Size(5, 5));

	vector<Mat> v(3);


	capture >> src;
	
	if (!src.data){
		printf("NO IMAGE\n");
		return -1;
	}

	while (capture.read(src)){

		//GaussianBlur(src, src, Size(3, 3), 0, 0);
		Canny(src, canny, 200, 400, 3);
		morphologyEx(canny, canny, MORPH_CLOSE, kernel);

		vector<Vec4i> lines;
		HoughLinesP(canny, lines, 1, CV_PI / 180, 150, 100, 100);

		for (i = 0; i < lines.size(); i++){
			Vec4i l = lines[i];
			temp_x1[i] = l[0];
			temp_x2[i] = l[2];
			temp_y1[i] = l[1];
			temp_y2[i] = l[3];

			float temp = (temp_y2[i] - temp_y1[i]) / (temp_x2[i] - temp_x1[i]);

			if ((temp<20) && (temp>0.6)){//Positive
				slide[count] = temp;
				x1[count] = temp_x1[i];
				x2[count] = temp_x2[i];
				y1[count] = temp_y1[i];
				y2[count] = temp_y2[i];
				
				if (slide[count]<positive){
					positive = slide[count];
					positive_key = count;
				}

				count++;

				if (count >= 100)
					count = 0;
			}
			else if ((temp>-20) && (temp<-0.6)){//Negative
				n_slide[n_count] = temp;
				n_x1[n_count] = temp_x1[i];
				n_x2[n_count] = temp_x2[i];
				n_y1[n_count] = temp_y1[i];
				n_y2[n_count] = temp_y2[i];

				if (n_slide[n_count] > negative){
					negative = n_slide[n_count];
					negative_key = n_count;
				}
				n_count++;

				if (n_count >= 100)
					n_count = 0;
			}

			line(src, Point(x1[positive_key], y1[positive_key]), Point(x2[positive_key], y2[positive_key]), Scalar(0, 255, 255), 3);
			line(src, Point(n_x2[negative_key], n_y2[negative_key]), Point(n_x1[negative_key], n_y1[negative_key]), Scalar(0, 255, 255), 3);
		}
		printf("1: %f, %f\n", positive, negative);
		printf("2: %d, %d\n", positive_key, negative_key);

		imshow("RESULT", src);
		imshow("Canny", canny);
		

		key = waitKey(frame_rate);

		if (key == 32){
			if (frame_rate == 30)
				frame_rate = 0;
			else if (frame_rate == 0)
				frame_rate = 30;
		}
		else if (key == 27)
			break;

	}

}

