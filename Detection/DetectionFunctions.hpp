#pragma once

namespace Detection {
	namespace ClickDetection {
		bool WasClickOnSquare(unsigned i, int x, int y); // click in square
	}

	class MotionDetection {
	public:
		void detectMovement(); // movement loop
		void detectJump();     // jump loop
	};
}
