#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "IOFunctions.hpp"
#include "Var/IOValues.hpp"
#include "../Graphics/GraphicsFunctions.hpp"
#include "../Graphics/Var/GraphicsValues.hpp"
#include "../Detection/DetectionFunctions.hpp"
#include "../Detection/Var/DetectionValues.hpp"
#include "../MapKeys/MapKeys.hpp"

using namespace std;
using namespace cv;

void IO::FileManager::readSavedSquares() {
    ifstream SavedSquares;
    int QntSquares;

    SavedSquares.open("data/SavedSquaresPositions.txt");

    if (SavedSquares.is_open()) {
        SavedSquares >> QntSquares;
        for (int i = 0; i < QntSquares; i++) {
            unsigned SquareX, SquareY, SquareKey;
            string SquareDisplayName;
            SavedSquares >> SquareX >> SquareY >> SquareKey >> SquareDisplayName;
            Graphics::ImageManager::createSquare(SquareX, SquareY, SquareKey, SquareDisplayName);
        }
        cout << "The saved squares were loaded successfully." << endl;
        SavedSquares.close();
    }
    else { cout << "'SavedSquaresPositions.txt' file does not exist, unable to load saved squares." << endl; }
}

void IO::FileManager::saveSquares() {
    try {
        ofstream SavedSquares("data/SavedSquaresPositions.txt");

        if (SavedSquares.is_open()) {

            SavedSquares << GraphicsValues::CVSquares::Squares.size() << endl;

            for (int i = 0; i < GraphicsValues::CVSquares::Squares.size(); i++) {

                SavedSquares << GraphicsValues::CVSquares::Squares[i].TL.x << endl;
                SavedSquares << GraphicsValues::CVSquares::Squares[i].TL.y << endl;
                SavedSquares << GraphicsValues::CVSquares::Squares[i].KEY << endl;
                SavedSquares << GraphicsValues::CVSquares::Squares[i].DISPLAYKEY << endl;
            }

            SavedSquares.close();
        }
    }
    catch (const exception& e) {
        cerr << "The program could not save properly to SavedSquaresPositions.txt, the program returned the following error: " << e.what() << endl;
    }
}

void IO::FileManager::readTolerance() {
    ifstream MoveTOLERANCE;
    MoveTOLERANCE.open("data/DetectionTolerance.txt");

    if (MoveTOLERANCE.is_open()) {
        MoveTOLERANCE >> GraphicsValues::TOLERANCE;
        cout << "The tolerance value was loaded successfully" << endl;
        MoveTOLERANCE.close();
    }
    else { cout << "'DetectionTolerance.txt' file does not exist, unable to load saved tolerance, using the default value." << endl; }
}

void IO::UserInput::DisplayInstructions() {

        cout << endl << "USAGE:" << endl;
        cout << endl << "HOW TO CREATE A SQUARE:" << endl
            << "- Double-click with the left mouse button to add a detection square at the clicked location." << endl;
        cout << endl << "HOW TO BIND A KEY:" << endl
            << "- Click on a square to select it." << endl
            << "- Press a key to bind it to the selected square." << endl;
        cout << endl << "HOW TO MOVE A SQUARE:" << endl
            << "- Click on a square to select it." << endl
            << "- Use arrow keys to move the square within the window." << endl;
        cout << endl << "HOW TO REMOVE A SQUARE:" << endl
            << "- Click on a square to select it." << endl
            << "- Right-click on a square to remove it." << endl;
        // Jump detection will be handled automatically; no prompt shown
}

bool IO::UserInput::activateJumpDetection() {
    // Default to enabling jump detection without prompting the user
    cout << endl << ">> Jump Detection ON\n\n\n\n" << endl;
    return true;
}

void IO::UserInput::onMouse(int event, int x, int y, int flags, void* userdata) {
    try {
        if (event == EVENT_LBUTTONDOWN) {

            if (DetectionValues::jumpDetectionActivated && GraphicsValues::CVJumpLine::Lines.size() != 0) {

                // Click on the jump line
                if (y >= GraphicsValues::CVJumpLine::Lines[0].Position.y - 15 && y <= GraphicsValues::CVJumpLine::Lines[0].Position.y + 15) {

                    GraphicsValues::CVJumpLine::Lines[0].SELECTED = true;
                }
                else if (GraphicsValues::CVJumpLine::Lines[0].SELECTED == true) {

                    GraphicsValues::CVJumpLine::Lines[0].SELECTED = false;
                }
            }

            if (GraphicsValues::CVSquares::Squares.size() != 0) {

                for (int i = 0; i < GraphicsValues::CVSquares::Squares.size(); i++) {

                    // Click on a square
                    if (Detection::ClickDetection::WasClickOnSquare(i, x, y)) {

                        if (DetectionValues::jumpDetectionActivated) {

                            GraphicsValues::CVJumpLine::Lines[0].SELECTED = false;
                        }

                        if (IOValues::selectedSquare == GraphicsValues::CVSquares::Squares[i].ID) { break; }
                        else {
                            for (auto& square : GraphicsValues::CVSquares::Squares) {
                                if (square.ID == IOValues::selectedSquare) {
                                    square.SELECTED = false;
                                    break;
                                }
                            }
                        }

                        IOValues::selectedSquare = GraphicsValues::CVSquares::Squares[i].ID;

                        GraphicsValues::CVSquares::Squares[i].SELECTED = true;

                        break;
                    }
                    else if (IOValues::selectedSquare != -1) {

                        for (auto& square : GraphicsValues::CVSquares::Squares) {
                            if (square.ID == IOValues::selectedSquare) {
                                square.SELECTED = false;
                                break;
                            }
                        }

                        IOValues::selectedSquare = -1;
                    }
                }
            }
        }
        if (event == EVENT_LBUTTONDBLCLK) {
            Graphics::ImageManager::createSquare(x, y, 0, "...");
        }
        if (event == EVENT_RBUTTONDOWN) {

            if (GraphicsValues::CVSquares::Squares.size() != 0 || IOValues::selectedSquare != -1) {

                for (int i = 0; i < GraphicsValues::CVSquares::Squares.size(); i++) {

                    if (Detection::ClickDetection::WasClickOnSquare(i, x, y)) {
                        try {
                            auto lambda = IOValues::selectedSquare;

                            GraphicsValues::CVSquares::Squares.erase(remove_if(GraphicsValues::CVSquares::Squares.begin(), GraphicsValues::CVSquares::Squares.end(),
                                [lambda](const GraphicsValues::CVSquares::Rectangles& square) { return square.ID == IOValues::selectedSquare; }), GraphicsValues::CVSquares::Squares.end());

                            GraphicsValues::CVSquares::Frames.erase(remove_if(GraphicsValues::CVSquares::Frames.begin(), GraphicsValues::CVSquares::Frames.end(),
                                [lambda](const GraphicsValues::CVSquares::Images& frame) { return frame.ID == IOValues::selectedSquare; }), GraphicsValues::CVSquares::Frames.end());

                            IOValues::selectedSquare = -1;

                            IO::FileManager::saveSquares();
                        }
                        catch (const exception& e) {
                            cerr << "it was not possible select the square, the method 'erase' returned it: " << e.what() << endl;
                        }
                    }
                }
            }
        }
    }
    catch (const exception& e) {
        cerr << "It was not possible to handle the mouse event, the program returned the following error: " << e.what() << endl;
    }
}

void IO::UserInput::CustomElementsOnWindow() {
    try {
        int keyPressed = waitKeyEx(10);

        if (GraphicsValues::CVJumpLine::Lines[0].SELECTED == true) {

            // Moves the jump line up
            if (keyPressed == 2490368) {

                if (GraphicsValues::CVJumpLine::Lines[0].Position.y > 5) {

                    GraphicsValues::CVJumpLine::Lines[0].Position.y -= 5;
                    Graphics::ImageManager::resizeJumpArea();
                }
            }
            // Moves the jump line down
            if (keyPressed == 2621440) {

                if (GraphicsValues::CVJumpLine::Lines[0].Position.y + 5 < GraphicsValues::CVMatFrames::imgFlip.size().height) {

                    GraphicsValues::CVJumpLine::Lines[0].Position.y += 5;
                    Graphics::ImageManager::resizeJumpArea();
                }
            }
        }
    }
    catch (const exception& e) {
        cerr << "It was not possible to custom the jump line height, the program returned the following error: " << e.what() << endl;
    }
}

void IO::UserInput::CustomElementsOnWindow(unsigned selectedSquare, IO::FileManager& fileManager) {
        try {
        int keyPressed = waitKeyEx(10);

        for (auto& square : GraphicsValues::CVSquares::Squares) {
            if (square.ID == selectedSquare) {

                // Move selected square left
                if (keyPressed == 2424832 && square.TL.x > 5) {

                    square.TL.x -= 5;

                    square.BR = Point(square.TL.x + 80, square.TL.y + 100);
                }
                // Move selected square up
                if (keyPressed == 2490368 && square.TL.y > 5) {

                    square.TL.y -= 5;

                    square.BR = Point(square.TL.x + 80, square.TL.y + 100);
                }
                // Move selected square right
                if (keyPressed == 2555904 && square.BR.x + 5 < GraphicsValues::CVMatFrames::imgFlip.size().width) {

                    square.TL.x += 5;

                    square.BR = Point(square.TL.x + 80, square.TL.y + 100);
                }
                // Move selected square down
                if (keyPressed == 2621440 && square.BR.y + 5 < GraphicsValues::CVMatFrames::imgFlip.size().height) {

                    square.TL.y += 5;

                    square.BR = Point(square.TL.x + 80, square.TL.y + 100);
                }

                // Assign key bind to the selected square
                if (MapKeys::keyMap.find(static_cast<char>(keyPressed)) != MapKeys::keyMap.end()) {

                    square.KEY = MapKeys::keyMap[static_cast<char>(keyPressed)];

                    square.DISPLAYKEY = std::string(1, toupper(static_cast<char>(keyPressed)));
                }
                // Special keys
                else {
                    // keyPressed for special keys (like SPACE, TAB) should be looked up as an int
                    auto itEx = MapKeys::keyMapEX.find(keyPressed);
                    if (itEx != MapKeys::keyMapEX.end()) {
                        square.KEY = itEx->second;

                        auto itName = MapKeys::keyEXName.find(keyPressed);
                        if (itName != MapKeys::keyEXName.end()) {
                            square.DISPLAYKEY = itName->second;
                        }
                        else {
                            square.DISPLAYKEY = std::to_string(keyPressed);
                        }
                    }
                }

                fileManager.saveSquares();

                break;
            }
        }
    }
    catch (const exception& e) {
        cerr << "It was not possible to custom the selected square, the program returned the following error: " << e.what() << endl;
    }
}