#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include "../IO/IOFunctions.hpp"

namespace Graphics {
	namespace CameraUtils {
		unsigned initializeCamera();
	}

	class ImageManager {
	public:

		static void createSquare(unsigned SquareX, unsigned SquareY, unsigned SquareKey, std::string SquareDisplayName);
		static void resizeJumpArea();

		void drawJumpLine(cv::Mat imgFlip);
		void preprocessImageAboveLine(cv::Mat imgFlip);
		void preprocessImage(cv::Mat imgFlip);
		void drawSquares(cv::Mat imgFlip);
	};
}