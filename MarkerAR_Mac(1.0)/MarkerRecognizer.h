#ifndef MarkerRecognizer_h
#define MarkerRecognizer_h

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>



class MarkerRecognizer
{
public:
    int marker_width;
    
    
public:
    void imageToColor(cv::Mat& image,cv::Mat& image_gray);
    void findMarkerContours(cv::Mat& image,cv::Mat& thresoldImage,int minContourPointsAllow);
    void updateMarker();
    void DrawQuard(cv::Mat& image,std::vector<cv::Point2f>& points,cv::Scalar& color);
    void clockwise(std::vector<cv::Point2f>& square);
    void perspective(std::vector<cv::Point2f>& square,cv::Mat& gray_image);
    void cameraCalibration(cv::Mat& image,std::vector<cv::Point2f>& square,cv::String& filePath);
};

#endif
