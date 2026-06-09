#pragma once

#include <fstream>
#include "../Graphics/GraphicsFunctions.hpp"
#include "../Graphics/Var/GraphicsValues.hpp"

namespace IO {
	class FileManager {
	public:
		static void readSavedSquares();

		// Save squares to disk
		static void saveSquares();
		static void readTolerance();
	};

	class UserInput {
	public:
		// Display usage instructions
		static void DisplayInstructions();

		// Ask to enable jump detection
		static bool activateJumpDetection();

		// Handle custom window interactions
		static void CustomElementsOnWindow();
		static void CustomElementsOnWindow(unsigned selectedSquare, IO::FileManager& fileManager);
		static void onMouse(int event, int x, int y, int flags, void* userdata);
	};
}