#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>

namespace DetectionValues {

    // Jump detection enabled
    extern bool jumpDetectionActivated;

    extern cv::Mat firstFrame;

    // Thresholds
    extern int PIXEL_THRESHOLD;       
    extern int MOVEMENT_THRESHOLD;    

    extern cv::Mat jumpFrame;
    extern int JUMP_THRESHOLD;

    // Jump state
    extern bool chargingJump;
    extern bool jumpStored;
    extern int storedJumpPower;
    extern bool executingJump;
    extern int currentDirectionKey; 
    extern int currentChargingMs; 
    extern std::chrono::steady_clock::time_point jumpStartTime;
}