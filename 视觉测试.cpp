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
		if (frame.empty())break;
		gray.create(frame.rows, frame.cols, CV_8UC1);
		MatIterator_<Vec3b> it1;
		MatIterator_<uchar> it2 = gray.begin<uchar>();
		for (it1 = frame.begin<Vec3b>(); it1 != frame.end<Vec3b>(); it1++)
		{
			if (abs((*it1)[0] - (*it1)[1])>0  || (*it1)[0] < 180 || ((*it1)[2] < (*it1)[1] ))
			{
				*it2 = 0;
				it2++;
				continue;
			}
			*it2 = 255;
			it2++;
		}
		imshow("gra", gray);
		cvWaitKey(3);
		vector<Vec4i> hierarchy;
		findContours(gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);		//CV_CHAIN_APPROX_SIMPLE ； CV_CHAIN_APPROX_NONE

		/// 对每个找到的轮廓创建可倾斜的边界框 
		vector<RotatedRect> minRect(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			float max_height = 0.0;
			RotatedRect Rect = minAreaRect(Mat(contours[i]));
			if (Rect.size.height < Rect.size.width)		//如果宽大于长，重新计算
			{
				Rect.angle += 90;
				float s = Rect.size.height;
				Rect.size.height = Rect.size.width;
				Rect.size.width = s;
				if (Rect.size.height > max_height)
					max_height = Rect.size.height;
			}
			float ratial = Rect.size.height / (float)Rect.size.width;

			if (Rect.angle > -30 && Rect.angle < 20 && (ratial > 3.2) && (ratial < 8))		//筛选角度、长宽比
				if ((Scalar)frame.at<Vec3b>(Rect.center) == Scalar(255, 255, 255))		//中心为纯白色
					if (Rect.size.height > (max_height / 2))		//不太小
					{
						minRect[i] = Rect;
					}
		}
		/// 绘出轮廓及其可倾斜的边界框
		for (int i = 0; i < contours.size(); i++)
		{
			// rotated rectangle  
			Point2f rect_points[4]; minRect[i].points(rect_points);
			for (int j = 0; j < 4; j++)
				line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(255,60,80), 2, 8);
		}

		/// 结果在窗体中显示  
		imshow("show", frame);
		cvWaitKey(10);

	}
	imshow("gray", gray);
	cvWaitKey(0);

	system("pause");
    return 0;
}


	
