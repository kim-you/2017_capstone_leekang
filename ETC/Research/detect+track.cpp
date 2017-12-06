
#include <opencv2/tracking.hpp>
#include <iostream>
#include <cstring>
#include < stdio.h>
#include < opencv2\opencv.hpp>
//#include "ml.hpp"
//#include < opencv2\gpu\gpu.hpp>

using namespace cv;
using namespace std;

#define  trackop TrackerKCF

/*
TrackerMedianFlow
TrackerMIL
TrackerBoosting
TrackerKCF
TrackerTLD
*/
float calcdist(float x1, float y1, float x2, float y2);
struct trackerstr {
	Point2d startpt;
	int startframe;
	Ptr<trackop> trackervector;
	Rect2d tROI;
};

void main()
{
	vector< float> descriptorsValues;
	vector< Point> locations;
	vector<Rect> rROI;
	vector<trackerstr> trackervec;
	vector<trackerstr>::iterator p;

	Size win_size(64, 64);
	HOGDescriptor d(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	Ptr<ml::SVM> svm = ml::SVM::create();
	svm = Algorithm::load<ml::SVM>("SVMab.xml");
	trackop::Params param;
	
	
	Mat frame, img_to_show;
	Mat c, grayc;

	Mat cluster;

	int framenum = 0;
	float result;
	auto upcount = 0;
	auto dncount = 0;
	auto stride = 0;
	VideoCapture v("sample16.mp4");

	
	
	/************************************************/
	auto zoom = 0.8;
	auto strider = 0.1;
	auto startxr = 0;
	auto startyr = 0.6;
	auto width = 1;
	auto height = 0.2;
	/************************************************/
	v >> frame;
	cluster = frame.clone();
	cvtColor(cluster, cluster, CV_RGB2GRAY);
	cluster.setTo(Scalar(0));
	resize(cluster, cluster, Size(frame.cols*zoom, frame.rows*zoom));
	
	imshow("SDFGDSg", cluster);
	while (1)
	{
		v >> frame;
		framenum++;
		if (!frame.empty()) {
			imshow("window", frame);
		}
		resize(frame, frame, Size(frame.cols*zoom, frame.rows*zoom));
		img_to_show = frame.clone();
		

		auto static startx = frame.cols*startxr;
		auto static starty = frame.rows*startyr;

		Rect troi(startx, starty, frame.cols*width, frame.rows*height);
		Rect btroi(startx, starty - 0.3*frame.rows*height, frame.cols*width, 1.6*frame.rows*height);
		auto static uplimit = starty; //-0.5*troi.height;
		auto static dnlimit = starty + 1*troi.height;
		rectangle(img_to_show, troi.tl(), troi.br(), CV_RGB(0, 0, 255), 2);
		rectangle(img_to_show, btroi.tl(), btroi.br(), CV_RGB(255, 0, 255), 2);
		auto ROIsize = frame.rows*height;

		for (int iterx = startx; ; ) {
			if (iterx + ROIsize > startx + troi.width)
				break;	
			Rect a(iterx, starty, ROIsize, ROIsize);
			c = frame(a);

			resize(c, c, Size(64, 64));
			cvtColor(c, grayc, CV_RGB2GRAY);

			d.compute(grayc, descriptorsValues);
			Mat fm = Mat(descriptorsValues);

			fm.convertTo(fm, CV_32F);
			fm = fm.reshape(0, 1);

			result = svm->predict(fm);
			if (result != -1) {
				rROI.push_back(a);
			}
			stride = ROIsize * strider;
			iterx += stride;
		}

		groupRectangles(rROI, 1, 0.3);
	
		for (int i = 0; i < rROI.size(); i++) {
			bool flag = false;
			for (int j = 0; j < trackervec.size(); j++) {
				if((rROI[i] & (Rect)(trackervec[j].tROI)).area() > 0.5 * rROI[i].area()){
					flag = true;
					break;
				}
			}
			if (!flag) {
				trackerstr temp;
				temp.startpt.x = rROI[i].x + 0.5*rROI[i].width;
				temp.startpt.y = rROI[i].y + 0.5*rROI[i].height;
				temp.startframe = framenum;
				temp.tROI = rROI[i];
				temp.trackervector = trackop::createTracker(param);
				temp.trackervector->init(frame, temp.tROI);
				trackervec.push_back(temp);
				//if(cluster.at<uchar>(Point(temp.startpt.x, temp.startpt.y))+ 100 > 255)
				//	cluster.at<uchar>(Point(temp.startpt.x, temp.startpt.y)) = 255;
				//else
				//	cluster.at<uchar>(Point(temp.startpt.x, temp.startpt.y)) += 100;
			}
	
			rectangle(img_to_show, rROI[i].tl(), rROI[i].br(), CV_RGB(0, 255, 0), 3);
			}
		//if (framenum % 100 == 0)
		//{
		//	cluster -= 20;
		//}




			for (int i = 0; i < trackervec.size(); i++) {
				trackervec[i].trackervector->update(frame, trackervec[i].tROI);
				if (trackervec[i].tROI.y + trackervec[i].tROI.height*0.5 < uplimit) {
					line(img_to_show, Point(0, uplimit), Point(frame.cols, uplimit), CV_RGB(255, 0, 0), 3);
					upcount++;
					float dist = calcdist(trackervec[i].tROI.x + 0.5*trackervec[i].tROI.width, trackervec[i].tROI.y + 0.5*trackervec[i].tROI.height, trackervec[i].startpt.x, trackervec[i].startpt.y);
					int btframe = framenum - trackervec[i].startframe;
					float speed = dist / btframe;
					cout << "상행  : " << upcount << "  " << dist << "/" << btframe<< " = " << speed << " \n";
					p = trackervec.begin() + i;
					trackervec.erase(p);
				}

				if (trackervec[i].tROI.y + trackervec[i].tROI.height*0.5 > dnlimit) {
					line(img_to_show, Point(0, dnlimit), Point(frame.cols, dnlimit), CV_RGB(255, 0, 0), 3);
					dncount++;
					float dist = calcdist(trackervec[i].tROI.x + 0.5*trackervec[i].tROI.width, trackervec[i].tROI.y + 0.5*trackervec[i].tROI.height, trackervec[i].startpt.x, trackervec[i].startpt.y);
					int btframe = framenum - trackervec[i].startframe;
					float speed = dist / btframe;
					cout << "\t\t\t\t\t하행  : " << dncount << "  " << dist << "/" << btframe << " = " << speed << " \n";
					p = trackervec.begin() + i;
					trackervec.erase(p);
				}

				rectangle(img_to_show, trackervec[i].tROI, Scalar(0, 0, 255), 2, 1);
			}

			rROI.clear();

			imshow("df", img_to_show);
			imshow("saf", frame);
			//imshow("asdf", cluster);
			waitKey(30);

			
		}
	}

	
	float calcdist(float x1, float y1, float x2, float y2) {
		return sqrt(((x1 - x2)*((x1 - x2)) + ((y1 - y2)*(y1 - y2))));
	}

