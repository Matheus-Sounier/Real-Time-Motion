#include <opencv2/opencv.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include "GraphicsFunctions.hpp"
#include "Var/GraphicsValues.hpp"
#include "../IO/IOFunctions.hpp"
#include "../Detection/Var/DetectionValues.hpp"

using namespace std;
using namespace cv;

static Ptr<CLAHE> g_clahe; // CLAHE shared instance

unsigned Graphics::CameraUtils::initializeCamera() {
    for (unsigned i = 0; i < 5; i++) {

        VideoCapture cap(i);

        if (cap.isOpened()) {

            cout << "Camera index chosen: " << i << endl;
            cap.release();

            return i;
        }
    }
    cerr << "The program could not find an available camera on the system" << endl;

    return 1;
}

void Graphics::ImageManager::createSquare(unsigned SquareX, unsigned SquareY, unsigned SquareKey, string SquareDisplayName) {
    if (SquareX > 0 && SquareY > 0 && SquareX + 80 <= GraphicsValues::CVMatFrames::imgFlip.size().width && SquareY + 100 <= GraphicsValues::CVMatFrames::imgFlip.size().height) {
        try {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<int> dis(1, 1000);

            GraphicsValues::CVSquares::Rectangles new_square{};
            new_square.TL = Point(SquareX, SquareY);
            new_square.BR = Point(SquareX + 80, SquareY + 100);
            new_square.COLOR = Scalar(0, 0, 255); // BGR FORMAT
            new_square.KEY = SquareKey;
            new_square.DISPLAYKEY = SquareDisplayName;
            new_square.SELECTED = false;
            new_square.MOTION_DETECTED = false;
            new_square.ID = dis(gen);

            GraphicsValues::CVSquares::Squares.push_back(new_square);

            GraphicsValues::CVSquares::Images new_frame{};
            new_frame.ID = new_square.ID;
            GraphicsValues::CVSquares::Frames.push_back(new_frame);

            IO::FileManager::saveSquares();
        }
        catch (const exception& e) {
            cerr << "It was not possible to create a square, the program returned the following error: " << e.what() << endl;
        }
    }
}

void Graphics::ImageManager::resizeJumpArea() {
    try {
        Size new_jumpareasize(GraphicsValues::CVMatFrames::imgFlip.size().width, GraphicsValues::CVJumpLine::Lines[0].Position.y);

        resize(GraphicsValues::CVMatFrames::JumpIMGCrop, GraphicsValues::CVMatFrames::JumpIMGCrop, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGGray, GraphicsValues::CVMatFrames::JumpIMGGray, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGBackground, GraphicsValues::CVMatFrames::JumpIMGBackground, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGThres, GraphicsValues::CVMatFrames::JumpIMGThres, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGSub, GraphicsValues::CVMatFrames::JumpIMGSub, new_jumpareasize);
        resize(GraphicsValues::CVMatFrames::JumpIMGDil, GraphicsValues::CVMatFrames::JumpIMGDil, new_jumpareasize);
    }
    catch (const exception& e) {
        cerr << "It was not possible to resize the jump detection image, the program returned the following error: " << e.what() << endl;
    }
}