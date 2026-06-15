#pragma once

namespace Detection {
	namespace ClickDetection {
		bool WasClickOnSquare(unsigned i, int x, int y);
	}

	class MotionDetection {
	public:
		void detectMovement();
		void detectJump();
	};
}
