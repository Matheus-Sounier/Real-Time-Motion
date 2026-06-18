#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>
#include "Detection/DetectionFunctions.hpp"
#include "Detection/Var/DetectionValues.hpp"
#include "IO/IOFunctions.hpp"
#include "IO/Var/IOValues.hpp"
#include "Graphics/GraphicsFunctions.hpp"
#include "Graphics/Var/GraphicsValues.hpp"

using namespace std;

int main() {

    IO::FileManager fileManager;
    IO::UserInput userInput;
    Detection::MotionDetection motionDetection;
    Graphics::ImageManager imageManager;

    int cam = Graphics::CameraUtils::initializeCamera();
    cv::VideoCapture cap(cam);
    cap.read(GraphicsValues::CVMatFrames::imgFlip);

    fileManager.readSavedSquares();

    fileManager.readTolerance();

    userInput.DisplayInstructions();

    DetectionValues::jumpDetectionActivated = userInput.activateJumpDetection();

    thread keyDetectionThread([&motionDetection]() {
        motionDetection.detectMovement();
    });

    thread jumpDetectionThread([&motionDetection]() {
        motionDetection.detectJump();
    });

    // Create window
    cv::namedWindow("Jump King IRL", cv::WINDOW_AUTOSIZE);
  
}