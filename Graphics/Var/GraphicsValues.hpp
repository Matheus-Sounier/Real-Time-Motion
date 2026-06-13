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

        extern std::vector<Images> Frames;
    }
}