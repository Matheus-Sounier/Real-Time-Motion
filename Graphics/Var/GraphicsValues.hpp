#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

namespace GraphicsValues {

    extern unsigned TOLERANCE;

    namespace CVMatFrames {
        extern cv::Mat img, imgFlip;
        extern cv::Mat JumpIMGCrop, JumpIMGGray, JumpIMGBackground, JumpIMGThres, JumpIMGSub, JumpIMGDil;
    }

    namespace CVSquares {

        /// Processed images for each square
        struct Images {
            cv::Mat imgCrop;
            cv::Mat imgDil;
            cv::Mat imgGray;
            cv::Mat imgThres;
            cv::Mat imgBackground;
            cv::Mat imgSub;
            int ID;
        };

        /// Square metadata
        struct Rectangles {
            cv::Point TL;
            cv::Point BR;
            cv::Scalar COLOR;
            int KEY;
            std::string DISPLAYKEY;
            bool SELECTED;
            bool MOTION_DETECTED;
            int ID;
        };

        extern std::vector<Images> Frames;
        extern std::vector<Rectangles> Squares;
        extern std::vector<std::vector<cv::Point>> contours;
    }
}