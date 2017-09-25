// 视觉测试.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "find.h"

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
	while (1)
	{
		cap >> frame;
		gray.create(frame.rows, frame.cols, CV_8UC1);
		MatIterator_<Vec3b> it1;
		MatIterator_<uchar> it2 = gray.begin<uchar>();
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
		findContours(gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);		//CV_CHAIN_APPROX_SIMPLE ； CV_CHAIN_APPROX_NONE

		//对每个轮廓的点集 找逼近多边形  
		vector<vector<Point>> approxPoint(contours.size());
		for (int i = 0; i < (int)contours.size(); i++)
		{
			approxPolyDP(contours[i], approxPoint[i], 1, true);
		}

		/******************************************绘制曲线********************************************/
		//用绘制轮廓的函数   绘制曲线  
		Mat drawImage = Mat::zeros(gray.size(), CV_8UC1);
		for (int i = 0; i < (int)contours.size(); i++)
		{
			if (contours[i].size() < 40)continue;
			drawContours(drawImage, contours, i, Scalar(255), 1);

		}
		//删除太少的元素
		vector<vector<Point>>::iterator itr = contours.begin();
		while (itr != contours.end())
		{
			if ((*itr).size() < 35)
				itr = contours.erase(itr);
			else
				itr++;
		}

		/// 对每个找到的轮廓创建可倾斜的边界框 
		vector<RotatedRect> minRect(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			RotatedRect Rect = minAreaRect(Mat(contours[i]));
			float ang = Rect.angle;
			if (Rect.size.height < Rect.size.width)
				ang += 90;
			if (ang > -30 && ang < 20)
				minRect[i] = Rect;
			cout << "angle" << ang << endl;

		}
		/// 绘出轮廓及其可倾斜的边界框和边界椭圆  
		Mat drawing = Mat::zeros(gray.size(), CV_8UC1);
		for (int i = 0; i < contours.size(); i++)
		{
			// rotated rectangle  
			Point2f rect_points[4]; minRect[i].points(rect_points);
			for (int j = 0; j < 4; j++)
				line(drawing, rect_points[j], rect_points[(j + 1) % 4], Scalar(255), 1, 8);
		}

		/// 结果在窗体中显示  
		imshow("Contours", drawing);
		imshow("show", frame);
		cvWaitKey(10);

	}
	imshow("gray", gray);
	cvWaitKey(0);

	system("pause");
    return 0;
}


	
