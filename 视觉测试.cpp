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
Point2f findtarget(const vector<RotatedRect>&  re);

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
			if (abs((*it1)[0] - (*it1)[1])>0  || (*it1)[0] < 140 || ((*it1)[2] < (*it1)[1] ))
			{
				*it2 = 0;
				it2++;
				continue;
			}
			*it2 = 255;
			it2++;
		}
		imshow("gray", gray);
		cvWaitKey(3);
		vector<Vec4i> hierarchy;
		findContours(gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);		//CV_CHAIN_APPROX_SIMPLE ； CV_CHAIN_APPROX_NONE

		/// 对每个找到的轮廓创建可倾斜的边界框 
		vector<RotatedRect> minRect;

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
					if (Rect.size.area() != 0.0)
						if (Rect.size.height > (max_height / 2))		//不太小
							minRect.push_back(Rect);
		}
		/// 绘出轮廓及其可倾斜的边界框
		for (int i = 0; i < minRect.size(); i++)
		{
			// rotated rectangle  
			Point2f rect_points[4]; 
			minRect[i].points(rect_points);
			for (int j = 0; j < 4; j++)
				line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(255,30,30), 2, 8);
		}


		// 结果在窗体中显示  
		for (int i = 0; i < minRect.size(); i++)
		{
			cout << "center" << minRect[i].center << endl;
			cout << "size" << minRect[i].size << endl;
			cout << "angle" << minRect[i].angle << endl << endl;
		}
		for (int i = 0; i < 10; i++)
			cout << endl;

		Point2f target= Point2f(0, 0);


		if (minRect.size()>2)
			for (int i = 0; i < minRect.size() - 1; i++)
			{
				for (int j = i + 1; j < minRect.size(); j++)
				{
					if (abs(minRect[i].angle - minRect[j].angle) < 6)		//角度
						if (abs(minRect[i].center.y - minRect[j].center.y) / max(minRect[i].size.height, minRect[j].size.height) < 0.5)	//竖直距离
							if (abs(minRect[i].size.height - minRect[j].size.height) / (minRect[i].size.height + minRect[j].size.height) < 0.3)		//高度差
								if (abs(minRect[i].center.x - minRect[j].center.x) / minRect[i].size.height < 3.5)
									if (abs(minRect[i].center.x - minRect[j].center.x) / minRect[i].size.height > 2)		//目标板的长宽比
										target=Point2f((minRect[i].center.x + minRect[j].center.x) / (float)2, (minRect[i].center.y + minRect[j].center.y) / (float)2);
										
				}
			}


		if (target != Point2f(0, 0))	//画出射击目标
			circle(frame, target, 20, Scalar(30, 255, 30), 2);

		imshow("show", frame);
		cvWaitKey(5);
		if (waitKey(10) == 27)
			system("pause");
		
	}

	system("pause");
    return 0;
}

	
