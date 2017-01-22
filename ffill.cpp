#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

IplImage* img;
CvPoint seedPoint;
CvScalar newColor = CV_RGB(250, 0, 0); // new color as red

void floodFill()
{
	//performing flood fill operation after mouse click
	cvFloodFill(img, seedPoint, newColor, cvScalarAll(3.5), cvScalarAll(3.5), NULL, 4, NULL);

	//showing image after flood fill operation
	cvShowImage("floodFill", img);
}

//mouse event handler
void onMouse(int event, int x, int y, int flags, void* param)
{
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		seedPoint = cvPoint(x, y); //setting mouse clicked location as seed point
		floodFill();
	}

}

int main(int argc, char** argv)
{	
	cvNamedWindow("floodFill");
	cvSetMouseCallback("floodFill", onMouse, 0);

	img = cvLoadImage("C:\\Users\\darke\\Desktop\\CV\\5.jpg", 1);

	//bluring the image
	//cvCanny(img, img, 10, 10);
	cvSmooth(img, img, CV_GAUSSIAN, 3,3,1);

	//showing original image
	cvShowImage("floodFill", img);

	cvWaitKey(0);
	return 0;
}