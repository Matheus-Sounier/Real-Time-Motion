#include <vector>
#include <opencv2/opencv.hpp>
#include <string>
#include "GraphicsValues.hpp"

cv::Mat GraphicsValues::CVMatFrames::img, GraphicsValues::CVMatFrames::imgFlip;

cv::Mat GraphicsValues::CVMatFrames::JumpIMGCrop, GraphicsValues::CVMatFrames::JumpIMGGray, GraphicsValues::CVMatFrames::JumpIMGBackground, GraphicsValues::CVMatFrames::JumpIMGThres, GraphicsValues::CVMatFrames::JumpIMGSub, GraphicsValues::CVMatFrames::JumpIMGDil;

std::vector<GraphicsValues::CVSquares::Images> GraphicsValues::CVSquares::Frames;

std::vector<std::vector<cv::Point>> GraphicsValues::CVSquares::contours;
std::vector<std::vector<cv::Point>> GraphicsValues::CVJumpLine::mycontours;

std::vector<GraphicsValues::CVSquares::Rectangles> GraphicsValues::CVSquares::Squares;

std::vector<GraphicsValues::CVJumpLine::Line> GraphicsValues::CVJumpLine::Lines;

bool GraphicsValues::CVJumpLine::LineExists = false;

// Minimum contour area to consider (tolerance)
unsigned GraphicsValues::TOLERANCE = 7;
