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
  
    // Program loop
    while (true) {
        cap.read(GraphicsValues::CVMatFrames::img);
        cv::flip(GraphicsValues::CVMatFrames::img, GraphicsValues::CVMatFrames::imgFlip, 1);

        // Handle mouse events
        cv::setMouseCallback("Jump King IRL", userInput.onMouse);

        imageManager.preprocessImage(GraphicsValues::CVMatFrames::imgFlip);
        imageManager.drawSquares(GraphicsValues::CVMatFrames::imgFlip);

    }
}