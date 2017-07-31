#include "cv.hpp" //Header
#include "opencv2/opencv.hpp" //Header include all

using namespace std;
using namespace cv;


#define ROTATION 3 //Define how many rotation it is
#define FRAMERATE 20 //Define fps

int number = 0;
int main(){
	int i = 0;
	int flag = -1;
	int average_edge = 0;
	int key, frame_rate = 30;

	VideoCapture capture("C:\\Users\\Administrator\\Desktop\\Study\\4학년\\공프기\\OpenCV\\TrafficExample\\traffic16.avi");
	
	Mat src, gray, edge, back, diff, back2, bob, bob2;

	vector<Mat> frames(5);

	capture >> src;


	if (!src.data){
		printf("NO IMAGE\n");
		return -1;
	}

	frames[0] = src.clone();
	frames[1] = src.clone();
	frames[2] = src.clone();
	while (capture.read(src)){
		
		resize(src, src, cv::Size(400, 300)); //Resize for better quality of view.
		
		flag++;
		if (flag == 0){
			back = src.clone();
			bob = back.clone();
			frames[0] = src.clone();
			frames[1] = src.clone();
			frames[2] = src.clone();
			continue;
		}


		cvtColor(src, gray, CV_BGR2GRAY);
		Canny(gray, edge, 50, 100);	
		cvtColor(back, back2, CV_BGR2GRAY);
		Canny(back2, back2, 50, 100);
	
		printf("%d\n", countNonZero(edge));

		if (countNonZero(edge) <= countNonZero(back2)*0.7 && flag >= 10){
			flag = 1;
			number++;
			number = number % ROTATION ;
			printf("=====================%d====================\n", number+1);
		}

		frames[number] = src.clone();

		char str[200];
		sprintf(str, "%d", number+1);
		putText(src, str, Point(10, 10), 1, 1, Scalar(0, 0, 255));

		imshow("RESULT", src);
		imshow("Canny", edge);
		imshow("VECTORTEST1", frames[0]);
		imshow("VECTORTEST2", frames[1]);
		imshow("VECTORTEST3", frames[2]);

		/*absdiff(src, back, diff);
		imshow("diff", diff);*/

		key = waitKey(frame_rate);
		
		if (key == 27 || key == 'q')
			break;

		if (key == 32){
			if (frame_rate == 30)
				frame_rate = 0;
			else if (frame_rate == 0)
				frame_rate = 30;
		}
		back = src.clone();
	}

}