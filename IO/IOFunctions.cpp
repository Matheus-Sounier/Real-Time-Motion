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
        cout << endl << "Enable jump detection? (send 'yes' or 'no')" << endl;
    //}
}

bool IO::UserInput::activateJumpDetection() {
    string JumpDetectionINPUT;

    cin >> JumpDetectionINPUT;

    if (JumpDetectionINPUT == "yes") {

        cout << endl << ">> Jump Detection ON\n\n\n\n" << endl;

        return true;
    }
    
    cout << endl << ">> Jump Detection OFF\n\n\n\n" << endl;
    return false;
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
                            cerr << "Nao foi possivel remover o quadrado selecionado, o metodo erase retornou o seguinte erro: " << e.what() << endl;
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