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

void Graphics::ImageManager::drawJumpLine(Mat imgFlip) {
    try {
        if (GraphicsValues::CVJumpLine::Lines.size() == 0) {

            GraphicsValues::CVJumpLine::Line new_jumpline;
            new_jumpline.Position = Point(0, 120);
            new_jumpline.COLOR = Scalar(255, 255, 255); // BGR FORMAT
            new_jumpline.SELECTED = false;

            GraphicsValues::CVJumpLine::Lines.push_back(new_jumpline);
        }

        if (GraphicsValues::CVJumpLine::Lines[0].SELECTED == true) {

            GraphicsValues::CVJumpLine::Lines[0].COLOR = Scalar(255, 0, 0); // BGR FORMAT
        }
        if (GraphicsValues::CVJumpLine::Lines[0].SELECTED == false) {

            GraphicsValues::CVJumpLine::Lines[0].COLOR = Scalar(255, 255, 255); // BGR FORMAT
        }

        line(imgFlip, GraphicsValues::CVJumpLine::Lines[0].Position, Point(imgFlip.size().width, GraphicsValues::CVJumpLine::Lines[0].Position.y), GraphicsValues::CVJumpLine::Lines[0].COLOR, 1.5);

        GraphicsValues::CVJumpLine::LineExists = true;

        try {
            const int barWidth = 300;
            const int barHeight = 18;
            int x = 20;
            int y = imgFlip.size().height - 40;

            if (DetectionValues::chargingJump) {
                int durationSoFar = DetectionValues::currentChargingMs;
                // background
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(50, 50, 50), cv::FILLED);
                // compute fraction using same clamps
                const int MIN_POWER = 50;
                const int MAX_POWER = 800;
                int clamped = durationSoFar;
                if (clamped < MIN_POWER) clamped = MIN_POWER;
                else if (clamped > MAX_POWER) clamped = MAX_POWER;
                float frac = (float)(clamped - MIN_POWER) / (float)(MAX_POWER - MIN_POWER);
                rectangle(imgFlip, Point(x+2, y+2), Point(x + 2 + static_cast<int>((barWidth-4)*frac), y + barHeight - 2), Scalar(0, 255, 255), cv::FILLED);
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(200,200,200), 1);
                char buf[64];
                snprintf(buf, sizeof(buf), "Charging: %d ms", durationSoFar);
                putText(imgFlip, buf, Point(x, y - 8), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(220,220,220), 1);
            }
            else if (DetectionValues::jumpStored) {
                // show stored power
                int stored = DetectionValues::storedJumpPower;
                // background
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(40, 40, 40), cv::FILLED);
                const int MIN_POWER = 50;
                const int MAX_POWER = 800;
                int clamped = stored;
                if (clamped < MIN_POWER) clamped = MIN_POWER;
                else if (clamped > MAX_POWER) clamped = MAX_POWER;
                float frac = (float)(clamped - MIN_POWER) / (float)(MAX_POWER - MIN_POWER);
                rectangle(imgFlip, Point(x+2, y+2), Point(x + 2 + static_cast<int>((barWidth-4)*frac), y + barHeight - 2), Scalar(180, 180, 0), cv::FILLED);
                rectangle(imgFlip, Point(x, y), Point(x + barWidth, y + barHeight), Scalar(200,200,200), 1);
                char buf[64];
                snprintf(buf, sizeof(buf), "Stored: %d ms", stored);
                putText(imgFlip, buf, Point(x, y - 8), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(220,220,220), 1);
                // indicate waiting for direction
                putText(imgFlip, "Select direction", Point(x + barWidth + 10, y + barHeight - 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200,200,200), 1);
            }
            else if (DetectionValues::executingJump) {
                putText(imgFlip, "Executing jump...", Point(x, y - 8), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(180, 200, 255), 1);
            }
        }
        catch (...) {
            // ignore
        }
    } 
    catch (const exception& e) {
        cerr << "It was not possible to draw the jump line, the program returned the following error: " << e.what() << endl;
    }
}

// Preprocess above jump line
void Graphics::ImageManager::preprocessImageAboveLine(Mat imgFlip) {
    try {
        Rect roi(Point(0, 0), Point(imgFlip.size().width, GraphicsValues::CVJumpLine::Lines[0].Position.y));

        GraphicsValues::CVMatFrames::JumpIMGCrop = imgFlip(roi);

        cvtColor(GraphicsValues::CVMatFrames::JumpIMGCrop, GraphicsValues::CVMatFrames::JumpIMGGray, COLOR_BGR2GRAY);

        // Apply CLAHE
        try {
            if (!g_clahe) g_clahe = createCLAHE(2.0, Size(8, 8));
            Mat tmpClahe;
            g_clahe->apply(GraphicsValues::CVMatFrames::JumpIMGGray, tmpClahe);
            GraphicsValues::CVMatFrames::JumpIMGGray = tmpClahe;
        }
        catch (...) {}

        if (GraphicsValues::CVMatFrames::JumpIMGBackground.empty()) {
            GraphicsValues::CVMatFrames::JumpIMGBackground = GraphicsValues::CVMatFrames::JumpIMGGray.clone();
        }

        addWeighted(GraphicsValues::CVMatFrames::JumpIMGBackground, 0.83, GraphicsValues::CVMatFrames::JumpIMGGray, 0.17, 0, GraphicsValues::CVMatFrames::JumpIMGBackground);

        absdiff(GraphicsValues::CVMatFrames::JumpIMGBackground, GraphicsValues::CVMatFrames::JumpIMGGray, GraphicsValues::CVMatFrames::JumpIMGSub);

        // Blur before threshold
        try {
            cv::Mat tmpBlur;
            GaussianBlur(GraphicsValues::CVMatFrames::JumpIMGSub, tmpBlur, Size(5, 5), 0);

            // Adjust threshold dynamically based on background brightness to reduce false positives
            double meanBg = 0.0;
            try { meanBg = cv::mean(GraphicsValues::CVMatFrames::JumpIMGBackground)[0]; } catch(...) { meanBg = 128.0; }
            int adaptiveThresh = DetectionValues::PIXEL_THRESHOLD;
            if (meanBg < 80.0) {
                adaptiveThresh = std::min(255, adaptiveThresh + static_cast<int>((80.0 - meanBg) / 2.0));
            }
            else if (meanBg > 180.0) {
                adaptiveThresh = std::max(1, adaptiveThresh - static_cast<int>((meanBg - 180.0) / 4.0));
            }

            threshold(tmpBlur, GraphicsValues::CVMatFrames::JumpIMGThres, adaptiveThresh, 255, THRESH_BINARY);
        }
        catch(...) {
            threshold(GraphicsValues::CVMatFrames::JumpIMGSub, GraphicsValues::CVMatFrames::JumpIMGThres, DetectionValues::PIXEL_THRESHOLD, 255, THRESH_BINARY);
        }

        // Morphology and dilate
        Mat kernelOpen = getStructuringElement(MORPH_RECT, Size(3, 3));
        try { morphologyEx(GraphicsValues::CVMatFrames::JumpIMGThres, GraphicsValues::CVMatFrames::JumpIMGThres, MORPH_OPEN, kernelOpen); } catch(...) {}

        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

        dilate(GraphicsValues::CVMatFrames::JumpIMGThres, GraphicsValues::CVMatFrames::JumpIMGDil, kernel);
    }
    catch (const exception& e) {
        cerr << "OpenCV could not process the jump detection image properly, the program returned the following error: " << e.what() << endl;
    }
}

// Preprocess squares
void Graphics::ImageManager::preprocessImage(Mat imgFlip) {
    try {
        for (int i = 0; i < GraphicsValues::CVSquares::Squares.size(); i++) {

            Rect roi(GraphicsValues::CVSquares::Squares[i].TL, Size(80, 100));

            GraphicsValues::CVSquares::Frames[i].imgCrop = imgFlip(roi);

            cvtColor(GraphicsValues::CVSquares::Frames[i].imgCrop, GraphicsValues::CVSquares::Frames[i].imgGray, COLOR_BGR2GRAY);

            if (GraphicsValues::CVSquares::Frames[i].imgBackground.empty()) {

                GraphicsValues::CVSquares::Frames[i].imgBackground = GraphicsValues::CVSquares::Frames[i].imgGray.clone();
            }

            // Apply CLAHE
            try {
                if (!g_clahe) g_clahe = cv::createCLAHE(2.0, cv::Size(8,8));
                cv::Mat tmpSq;
                g_clahe->apply(GraphicsValues::CVSquares::Frames[i].imgGray, tmpSq);
                GraphicsValues::CVSquares::Frames[i].imgGray = tmpSq;
            }
            catch (...) {}

            addWeighted(GraphicsValues::CVSquares::Frames[i].imgBackground, 0.83, GraphicsValues::CVSquares::Frames[i].imgGray, 0.17, 0, GraphicsValues::CVSquares::Frames[i].imgBackground);

            absdiff(GraphicsValues::CVSquares::Frames[i].imgBackground, GraphicsValues::CVSquares::Frames[i].imgGray, GraphicsValues::CVSquares::Frames[i].imgSub);

            // Blur and threshold
            try {
                cv::Mat tmpBlurSq;
                GaussianBlur(GraphicsValues::CVSquares::Frames[i].imgSub, tmpBlurSq, Size(5, 5), 0);

                double meanBgSq = 0.0;
                try { meanBgSq = cv::mean(GraphicsValues::CVSquares::Frames[i].imgBackground)[0]; } catch(...) { meanBgSq = 128.0; }
                int adaptiveThreshSq = DetectionValues::PIXEL_THRESHOLD;
                if (meanBgSq < 80.0) {
                    adaptiveThreshSq = std::min(255, adaptiveThreshSq + static_cast<int>((80.0 - meanBgSq) / 2.0));
                }
                else if (meanBgSq > 180.0) {
                    adaptiveThreshSq = std::max(1, adaptiveThreshSq - static_cast<int>((meanBgSq - 180.0) / 4.0));
                }

                threshold(tmpBlurSq, GraphicsValues::CVSquares::Frames[i].imgThres, adaptiveThreshSq, 255, THRESH_BINARY);
            }
            catch(...) {
                threshold(GraphicsValues::CVSquares::Frames[i].imgSub, GraphicsValues::CVSquares::Frames[i].imgThres, DetectionValues::PIXEL_THRESHOLD, 255, THRESH_BINARY);
            }

            // Morphology and dilate
            Mat kernelOpen = getStructuringElement(MORPH_RECT, Size(3, 3));
            try { morphologyEx(GraphicsValues::CVSquares::Frames[i].imgThres, GraphicsValues::CVSquares::Frames[i].imgThres, MORPH_OPEN, kernelOpen); } catch(...) {}

            Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));

            dilate(GraphicsValues::CVSquares::Frames[i].imgThres, GraphicsValues::CVSquares::Frames[i].imgDil, kernel);

            putText(imgFlip, GraphicsValues::CVSquares::Squares[i].DISPLAYKEY, Point(GraphicsValues::CVSquares::Squares[i].TL.x + 10, GraphicsValues::CVSquares::Squares[i].TL.y + 45), FONT_HERSHEY_DUPLEX, 0.5, GraphicsValues::CVSquares::Squares[i].COLOR, 1.99);
        }
    }
    catch (const exception& e) {
        cerr << "OpenCV could not process the image properly, the program returned the following error: " << e.what() << endl;
    }
}

// Draw squares
void Graphics::ImageManager::drawSquares(Mat imgFlip) {
    try {
        if (GraphicsValues::CVSquares::Squares.size() != 0) {

            for (int i = 0; i < GraphicsValues::CVSquares::Squares.size(); i++) {

                Scalar SquareCOLOR = GraphicsValues::CVSquares::Squares[i].COLOR;

                rectangle(imgFlip, GraphicsValues::CVSquares::Squares[i].TL, GraphicsValues::CVSquares::Squares[i].BR, SquareCOLOR, 3);
            }
        }
    }
    catch (const exception& e) {
        cerr << "It was not possible to draw the squares properly, the program returned the following error: " << e.what() << endl;
    }
}