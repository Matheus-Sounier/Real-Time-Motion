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
            cv::Mat imgCrop;       ///< Cropped image of the square.
            cv::Mat imgDil;        ///< Dilated image of the square.
            cv::Mat imgGray;       ///< Grayscale image of the square.
            cv::Mat imgThres;      ///< Thresholded image of the square.
            cv::Mat imgBackground; ///< Background image of the square.
            cv::Mat imgSub;        ///< Subtracted image of the square.
            int ID;                ///< Random ID of the square.
        };

        /// Square metadata
        struct Rectangles {
            cv::Point TL;                ///< Top-left corner of the square.
            cv::Point BR;                ///< Bottom-right corner of the square.
            cv::Scalar COLOR;            ///< Color of the square.
            int KEY;                     ///< Key associated with the square.
            std::string DISPLAYKEY;      ///< Display key associated with the square.
            bool SELECTED;               ///< Indicates whether the square is selected.
            bool MOTION_DETECTED;        ///< Indicates whether motion is detected within the square.
            int ID;                      ///< Random ID of the square.
        };

        extern std::vector<Images> Frames;
        extern std::vector<Rectangles> Squares;
        extern std::vector<std::vector<cv::Point>> contours;
    }

    namespace CVJumpLine {

        struct Line {
            cv::Point Position;
            cv::Scalar COLOR;
            bool SELECTED = false;
        };

        extern std::vector<Line> Lines;
        extern bool LineExists;
        extern const int W;
        extern const int Y;
        extern std::vector<std::vector<cv::Point>> mycontours;
    }
}