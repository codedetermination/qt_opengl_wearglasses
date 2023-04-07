#ifndef USE_CAMERA_H
#define USE_CAMERA_H
#include <opencv4/opencv2/opencv.hpp>


class MyCamera{
public:
    MyCamera(int cam = 0):index(cam){
        cap.open(camnow);
        if(!cap.isOpened()){
            throw std::runtime_error("Could not open camera.");
        }
    }
    void switch_camera(){
        if(index >0){
            if(camnow == 0){
                camnow = 1;
            }
            else{
                camnow =0;
            }
        }
        cap.open(camnow);
    };
    cv::VideoCapture cap;
    int index;
    int camnow = 0;
    cv::Mat cap_image(){
        cv::Mat frame;
        cap >> frame;
        return frame;
    }


};


#endif // USE_CAMERA_H
