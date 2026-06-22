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
    // Initialize instances of core classes
    IO::FileManager fileManager;
    IO::UserInput userInput;
    Detection::MotionDetection motionDetection;
    Graphics::ImageManager imageManager;

    // Initialize camera
    int cam = Graphics::CameraUtils::initializeCamera();
    cv::VideoCapture cap(cam);
    cap.read(GraphicsValues::CVMatFrames::imgFlip);

    // Load saved squares
    fileManager.readSavedSquares();

    // Load tolerance value
    fileManager.readTolerance();

    // Display instructions
    userInput.DisplayInstructions();

    // Ask user to enable jump detection
    DetectionValues::jumpDetectionActivated = userInput.activateJumpDetection();

    // Start detection threads
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
        // Capture frame
        cap.read(GraphicsValues::CVMatFrames::img);
        cv::flip(GraphicsValues::CVMatFrames::img, GraphicsValues::CVMatFrames::imgFlip, 1);

        // Handle mouse events
        cv::setMouseCallback("Jump King IRL", userInput.onMouse);

        // Preprocess frames and draw UI
        imageManager.preprocessImage(GraphicsValues::CVMatFrames::imgFlip);
        imageManager.drawSquares(GraphicsValues::CVMatFrames::imgFlip);

        if (DetectionValues::jumpDetectionActivated) {

            imageManager.drawJumpLine(GraphicsValues::CVMatFrames::imgFlip);
            imageManager.preprocessImageAboveLine(GraphicsValues::CVMatFrames::imgFlip);
        }

        // Move elements and handle key bindings
        if (IOValues::selectedSquare != -1) {

            userInput.CustomElementsOnWindow(IOValues::selectedSquare, fileManager);
        }
        else if (DetectionValues::jumpDetectionActivated) {

            if (GraphicsValues::CVJumpLine::Lines[0].SELECTED == true) {

                userInput.CustomElementsOnWindow();
            }
        }
        
        cv::imshow("Jump King IRL", GraphicsValues::CVMatFrames::imgFlip);

        cv::waitKey(1);
    }

    // Join threads and release camera
    keyDetectionThread.join();
    jumpDetectionThread.join();
    cap.release();

    return 0;
}