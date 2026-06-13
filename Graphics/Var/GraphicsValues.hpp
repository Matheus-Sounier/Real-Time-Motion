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
}