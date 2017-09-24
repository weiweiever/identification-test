// 视觉测试.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat frame,gray;
vector<vector<Point> > contours;


int main()
{
	VideoCapture cap("G://im//21.avi");
	if (!cap.isOpened())
	{
		cout << "无法加载此文件." << endl;
	}
	cap >> frame;
	gray.create(frame.rows, frame.cols, CV_8UC1);
	MatIterator_<Vec3b> it1;
	MatIterator_<uchar> it2=gray.begin<uchar>();
	for (it1 = frame.begin<Vec3b>(); it1 != frame.end<Vec3b>(); it1++)
	{
		if (abs((*it1)[0] - (*it1)[1]) > 3)
		{
			*it2 = 0;
			it2++;
			continue;
		}
		if ((*it1)[2] / (float)(*it1)[1] != 1)
		{
			*it2 = 0;
			it2++;
			continue;
		}
		*it2 = 255;
		it2++;
	}
	vector<Vec4i> hierarchy;
	findContours(gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Mat imageContours = Mat::zeros(gray.size(), CV_8UC1);
	Mat Contours = Mat::zeros(gray.size(), CV_8UC1);  //绘制  
	for (int i = 0; i < contours.size(); i++)
	{
		//contours[i]代表的是第i个轮廓，contours[i].size()代表的是第i个轮廓上所有的像素点数  
		for (int j = 0; j < contours[i].size(); j++)
		{
			//绘制出contours向量内所有的像素点  
			Point P = Point(contours[i][j].x, contours[i][j].y);
			Contours.at<uchar>(P) = 255;
		}
		drawContours(imageContours, contours, i, Scalar(255), 1, 8, hierarchy);
	}
	imshow("Contours Image", imageContours); //轮廓  
	imshow("Point of Contours", Contours);   //向量contours内保存的所有轮廓点集  
	waitKey(0);
	imshow("gray", gray);
	cvWaitKey(0);
	imshow("show", frame);
	cvWaitKey(0);

	system("pause");
    return 0;
}


	
