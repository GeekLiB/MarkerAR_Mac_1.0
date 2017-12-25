#include "MarkerRecognizer.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

MarkerRecognizer m_recognisze;

int main() {
    
    VideoCapture cap(0);
    
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 280);
    
    while (1) {
        
        Mat image,image_gray;
        cap>>image;
        m_recognisze.imageToColor(image, image_gray);
        
        imshow("ss", image);
        
        int c = cv::waitKey(20);
        if((char)c == 'q')
        {
            break;
        }
        if(c >= 0 && c!= 'q')
        {
            cv::waitKey();
        }
    }
    
    return 0;
}
