#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>

namespace DetectionValues {

    // Jump detection enabled
    extern bool jumpDetectionActivated;

    // Background frame for movement detection
    extern cv::Mat firstFrame;

    // Thresholds
    extern int PIXEL_THRESHOLD;       // threshold for binarization
    extern int MOVEMENT_THRESHOLD;    // area threshold for movement in ROIs

    // Jump detection frame and threshold
    extern cv::Mat jumpFrame;
    extern int JUMP_THRESHOLD;

    // Jump state
    extern bool chargingJump;
    extern bool jumpStored;
    extern int storedJumpPower; // ms
    extern bool executingJump;
    extern int currentDirectionKey; // chosen direction while executing
    extern int currentChargingMs; // charging time for UI
    extern std::chrono::steady_clock::time_point jumpStartTime;
}