#include "DetectionValues.hpp"

namespace DetectionValues {

    bool jumpDetectionActivated = false;

    cv::Mat firstFrame;
    int PIXEL_THRESHOLD = 30;
    int MOVEMENT_THRESHOLD = 1000;

    cv::Mat jumpFrame;
    int JUMP_THRESHOLD = 1200;

    bool chargingJump = false;
    bool jumpStored = false;
    int storedJumpPower = 0;
    bool executingJump = false;
    int currentDirectionKey = 0;
    int currentChargingMs = 0;
    
    std::chrono::steady_clock::time_point jumpStartTime;
}