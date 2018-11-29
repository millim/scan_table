#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <vector>
#include <numeric>
#include <xlocale.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/aruco.hpp>

using namespace cv;
using namespace std;
void sm(cv::Mat &img)
{
  imshow("sm show", img);
  cv::waitKey();
}

int main(int argc, const char *argv[])
{

  cv::Mat img = cv::imread("./201811231546039232image.jpg");
  GaussianBlur(img, img, Size(3, 3), 0, 0);
  cvtColor(img, img, CV_BGR2GRAY);
  int height = img.rows;
  int width = img.cols;
  double scale;
  if (height >= width)
  {
    scale = 800.0 / height;
  }
  else
  {
    scale = 800.0 / width;
  }
  cv::resize(img, img, cv::Size(width * scale, height * scale), 0, 0, CV_INTER_AREA);

  Mat binary_image;
  adaptiveThreshold(img, binary_image, 255, CV_ADAPTIVE_THRESH_MEAN_C,
                    CV_THRESH_BINARY_INV, 25, 10);

  vector<vector<Point>> contours;
  vector<Rect> boundRect(contours.size());
  findContours(binary_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

  vector<Point> contour = contours[0];
  RotatedRect rect = minAreaRect(contour);

  // Point2f fourPoint[4];        //定义一个存储以上4个点的坐标的变量
  // rectPoint.points(fourPoint); //将rectPoint变量中存储的坐标值 放到fourPoint的数组中
  // int minX = fourPoint[0].x;
  // int minY = fourPoint[0].y;
  // int maxX = fourPoint[0].x;
  // int maxY = fourPoint[0].y;

  Point2f vertices[4];
  rect.points(vertices);

  int minX = vertices[0].x;
  int minY = vertices[0].y;
  int maxX = vertices[0].x;
  int maxY = vertices[0].y;

  for (int i = 0; i < contours.size(); i++) //遍历每一个轮廓
  {
    //获取（第i+1个）外接矩形的4个顶点坐标
    RotatedRect rectPoint = minAreaRect(contours[i]);
    Point2f fourPoint[4];        //定义一个存储以上4个点的坐标的变量
    rectPoint.points(fourPoint); //将rectPoint变量中存储的坐标值 放到fourPoint的数组中

    for (int j = 0; j < 4; j++)
    {
      if (fourPoint[j].x < minX)
      {
        minX = fourPoint[j].x;
      }
      if (fourPoint[j].x > maxX)
      {
        maxX = fourPoint[j].x;
      }
      if (fourPoint[j].y < minY)
      {
        minY = fourPoint[j].y;
      }
      if (fourPoint[j].y > maxY)
      {
        maxY = fourPoint[j].y;
      }
    }
  }

  Mat dd(img.rows - 1, img.cols - 1, CV_32F);
  cv::drawContours(dd,
                   contours,
                   0,
                   Scalar(255, 255, 255));

  vector<vector<Point>> contours2;

  Mat cornerStrength;
  goodFeaturesToTrack(dd, cornerStrength, 10, 12, 0.1);

  //对灰度图进行阈值操作，得到二值图并显示
  Mat harrisCorner;
  threshold(cornerStrength, harrisCorner, 0.00001, 255, THRESH_BINARY);
  sm(harrisCorner);

  // IplImage *srcImage = 0, *grayImage = 0, *corners1 = 0, *corners2 = 0;
  // cvGoodFeaturesToTrack(grayImage, corners1, corners2, corners, &cornersCount, 0.05, 30, 0, 3, 0, 0.4);
  // cout << c << endl;

  sm(dd);
  // circle(img, Point(minX, minY), 10, CV_RGB(0, 255, 255), 2, 8, 0);

  Point2f left_top, right_top, left_bottom, right_bottom;
  for (int i = 0; i < 4; i++)
  {
    cout << vertices[i].x << "," << vertices[i].y << endl;
    //画圆 cvPoint:确定圆的坐标  200：圆的半径 CV_RGB：圆的颜色 3：线圈的粗细
    circle(img, vertices[i], 10, CV_RGB(0, 255, 255), 2, 8, 0);
  }
  sm(img);
  if (vertices[0].x > vertices[2].x && vertices[0].y > vertices[2].y)
  {

    left_top = vertices[2];
    right_top = vertices[3];
    left_bottom = vertices[1];
    right_bottom = vertices[0];
  }
  else
  {
    left_top = vertices[1];
    right_top = vertices[2];
    left_bottom = vertices[0];
    right_bottom = vertices[3];
  }

  double trheight = sqrt(abs(left_top.x - left_bottom.x) * abs(left_top.x - left_bottom.x) +
                         abs(left_top.y - left_bottom.y) * abs(left_top.y - left_bottom.y));

  double trwidth = sqrt(abs(left_top.x - right_top.x) * abs(left_top.x - right_top.x) +
                        abs(left_top.y - right_top.y) * abs(left_top.y - right_top.y));

  cv::Point2f toPoints[4], fromPoints[4];

  fromPoints[0] = left_top;
  toPoints[0] = Point2f(0.0, 0.0);

  fromPoints[1] = right_top;
  toPoints[1] = Point2f(0.0 + trwidth, 0.0);

  fromPoints[2] = left_bottom;
  toPoints[2] = Point2f(0.0, 0.0 + trheight);

  fromPoints[3] = right_bottom;
  toPoints[3] = Point2f(0.0 + trwidth, 0.0 + trheight);

  cv::Mat transform = cv::getPerspectiveTransform(fromPoints, toPoints);

  Mat momo;
  cv::warpPerspective(img,
                      momo,
                      transform,
                      cv::Size(trwidth, trheight));

  sm(img);
  sm(momo);

  return 0;
}