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