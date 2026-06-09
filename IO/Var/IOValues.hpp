#pragma once

#include <vector>
#include <string>

namespace IOValues {

	struct Square
	{
		int X;
		int Y;
		int WIDTH;
		int HEIGHT;

		int KEY;

		bool DETECTED;
		bool KEY_PRESSED;
	};

	extern std::vector<Square> Squares;

	extern int selectedSquare;
	extern bool clickOnSquare;
}