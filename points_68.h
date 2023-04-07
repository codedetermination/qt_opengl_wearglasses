#ifndef POINTS_68_H
#define POINTS_68_H
#include <iostream>
#include <fstream>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/opencv.h>
//#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QFile>
#include <QString>

using namespace std ;
using namespace dlib;


class points_68{
public:
    points_68(cv::Mat img1) : image(img1){

    }
    cv::Mat image;
    cv::Mat return_image(){
        cv::Mat img = image;
        if (img.empty())
        {
            cerr << "Error: Could not read input image" << endl;
        }
        //cv::imshow("initial_image",image);
        // Convert image to dlib's format
        cv_image<rgb_pixel> img_rgb(img);


        // Initialize face detector and landmark predictor objects
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor predictor;
//        QString filename = "/home/guoyi/Desktop/shejigame/wearglasses1/shape_predictor_68_face_landmarks.dat";
//        QFile file(filename);
//        if (!file.open(QIODevice::ReadOnly))
//        {
//            // Handle error opening file...
//            cerr << "Error: Could not read input file" << endl;
//        }
        deserialize("/home/guoyi/Desktop/shejigame/wearglasses1/shape_predictor_68_face_landmarks.dat") >> predictor;
        //QByteArray data = file.readAll();
        //dlib::deserialize(data.constData()) >> predictor;
//        file.close();
        // Detect faces in image
        std::vector<rectangle> faces = detector(img_rgb);

        // Extract landmarks for each face
        std::vector<full_object_detection> landmarks;
        for (const auto& face : faces)
            landmarks.push_back(predictor(img_rgb, face));

        // Save landmarks to .pts file
        ofstream pts_file("output.pts");
        pts_file << "version: 1" << endl << "n_points: 68" << endl << "{" << endl;
        for (const auto& face_landmarks : landmarks)
        {
            for (unsigned long i = 0; i < face_landmarks.num_parts(); ++i)
            {
                point pt = face_landmarks.part(i);
                pts_file << pt.x() << " " << pt.y() << " 0.0" << endl;
            }
        }
        pts_file << "}" << endl;
        pts_file.close();

        // Visualize landmarks on image
        for (const auto& face_landmarks : landmarks)
        {
            for (unsigned long i = 0; i < face_landmarks.num_parts(); ++i)
            {
                point pt = face_landmarks.part(i);
                cv::circle(img, cv::Point(pt.x(), pt.y()), 2, cv::Scalar(0, 255, 0), -1);
            }
        }
        //cv::imshow("img",img);
        cv::waitKey(0);
        return img;
    }
};

#endif // POINTS_68_H
