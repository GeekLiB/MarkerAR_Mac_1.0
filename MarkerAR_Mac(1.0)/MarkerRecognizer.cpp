#include "MarkerRecognizer.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <math.h>
#include <string>
#include <sstream>
#include <stdio.h>



using namespace std;
using namespace cv;
Scalar blue(255,0,0);
Scalar red(0,0,255);
Scalar green(0,255,0);

//replace your camera data
String filepath="/Users/albertlee/Documents/out_camera_data.xml";

void MarkerRecognizer::imageToColor(Mat& image, Mat& image_gray)
{
    
    cvtColor(image, image_gray, CV_BGR2GRAY);
    Mat blur_image;
    blur(image_gray, blur_image, Size(5,5));
    Mat threshold_image;
    
    threshold(blur_image, threshold_image, 128.0, 255.0, THRESH_OTSU);
    findMarkerContours(image, threshold_image, 100);
    
}

void MarkerRecognizer::findMarkerContours(Mat& image,Mat& thresoldImage , int minContourPointsAllow)
{
    vector<vector<Point>> allcontours;
    vector<vector<Point>> contours;
    findContours(thresoldImage, allcontours,CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    contours.clear();
    for(int i=0;i<allcontours.size();i++)
    {
        int contourSize=allcontours[i].size();
        if(contourSize>minContourPointsAllow)
        {
            contours.push_back(allcontours[i]);
        }
    }
    //drawContours(image, contours, -1, Scalar(255,0,0));
    vector<vector<Point2f>> squares;
    for(int i=0;i<contours.size();i++)
    {
        vector<Point> contour = contours[i];
        vector<Point> approx;
        approxPolyDP(contour, approx, 5, true);
        if(approx.size()==4)
        {
            vector<Point2f> square;
            for(int i=0;i<4;i++)
            {
                square.push_back(Point2f(approx[i].x,approx[i].y));
            }
            squares.push_back(square);
        }
    }
    
    vector<Point2f> square = squares[0];
    DrawQuard(image, square, blue);
    Mat gray_image=image.clone();
    cvtColor(gray_image, gray_image, CV_BGR2GRAY);
    perspective(square,gray_image);
    circle(image, square[0], 5, red);
    cameraCalibration(image, square, filepath);
    
}

void MarkerRecognizer::DrawQuard(Mat& image, vector<Point2f>& points, Scalar& color)
{
    line(image, points[0], points[1], color);
    line(image, points[1], points[2], color);
    line(image, points[2], points[3], color);
    line(image, points[4], points[4], color);
}

void MarkerRecognizer::clockwise(vector<Point2f>& square)
{
    Point2f v1=square[1]-square[0];
    Point2f v2=square[2]-square[0];
    
    double o =(v1.x*v2.y)-(v1.y*v2.x);
    
    if(o<0.0)
    {
        std::swap(square[1], square[3]);
    }
}


void MarkerRecognizer::perspective(vector<Point2f>& square,Mat& gray_image)
{
    clockwise(square);
    
    Mat marker;
    vector<Point2f> marker_square;
    
    marker_square.push_back(Point(0,0));
    marker_square.push_back(Point(marker_width-1, 0));
    marker_square.push_back(Point(marker_width-1,marker_width-1));
    marker_square.push_back(Point(0, marker_width-1));
    
    
    Mat transform = getPerspectiveTransform(square, marker_square);
    warpPerspective(gray_image, marker,transform, Size(marker_width,marker_width));
    threshold(marker, marker, 125, 255, THRESH_BINARY|THRESH_OTSU);
    
    vector<Point> direction_point = {{50, 50} ,{150, 50},{150, 150},{50,150}};
    int direction;
    for (int i = 0; i < 4; ++i){
        Point p = direction_point[i];
        if (countNonZero(marker(Rect(p.x-25,p.y-25,marker_width/4,marker_width/4))) >20){
            direction = i;
            break;
        }
    }
    for (int i = 0; i < direction; ++i){
        rotate(square.begin(), square.begin() + 1, square.end());
    }
}



void MarkerRecognizer::cameraCalibration(Mat& image,vector<Point2f>& square, String& filePath)
{
    FileStorage fs(filePath,FileStorage::READ);
    
    Mat intrinsics, distortion;
    fs["Camera_Matrix"] >> intrinsics;
    fs["Distortion_Coefficients"] >> distortion;
    
    vector<Point3f> objectPoints;
    objectPoints.push_back(Point3f(-1, 1, 0));
    objectPoints.push_back(Point3f(1, 1, 0));
    objectPoints.push_back(Point3f(1, -1, 0));
    objectPoints.push_back(Point3f(-1, -1, 0));
    Mat objectPointsMat(objectPoints);
    
    Mat rvec;
    Mat tvec;
    
    solvePnP(objectPointsMat, square, intrinsics, distortion, rvec, tvec);
    
    cout << "rvec: " << rvec << endl;
    cout << "tvec: " << tvec << endl;
    
    vector<Point3f> line3dx = {{0, 0, 0}, {2, 0, 0}};
    vector<Point3f> line3dy = {{0, 0, 0}, {0, 2, 0}};
    vector<Point3f> line3dz = {{0, 0, 0}, {0, 0, 2}};
    
    
    vector<Point2f> line2dx;
    vector<Point2f> line2dy;
    vector<Point2f> line2dz;
    
    projectPoints(line3dx, rvec, tvec, intrinsics, distortion, line2dx);
    projectPoints(line3dy, rvec, tvec, intrinsics, distortion, line2dy);
    projectPoints(line3dz, rvec, tvec, intrinsics, distortion, line2dz);
    
    line(image, line2dx[0], line2dx[1], red);
    line(image, line2dy[0], line2dy[1], blue);
    line(image, line2dz[0], line2dz[1], green);
    
    
    
}
