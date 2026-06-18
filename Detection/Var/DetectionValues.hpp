#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>

namespace DetectionValues {

    extern bool jumpDetectionActivated;

    extern cv::Mat firstFrame;

    extern int PIXEL_THRESHOLD;
    extern int MOVEMENT_THRESHOLD;

    // Jump detection frame and threshold
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