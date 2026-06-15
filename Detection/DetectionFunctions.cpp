#include "DetectionFunctions.hpp"

#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>
#include <cctype>
#include <cstdio>
#include <unordered_map>

#include "Var/DetectionValues.hpp"
#include "../Graphics/Var/GraphicsValues.hpp"
#include "../MapKeys/MapKeys.hpp"

using namespace std::chrono_literals;
using namespace Detection;
using namespace DetectionValues;
using namespace GraphicsValues;

namespace Detection {

	static int normalize_vk(int key) {
		if (key >= 'a' && key <= 'z') return std::toupper(static_cast<unsigned char>(key));
		return key;
	}

	namespace ClickDetection {
		bool WasClickOnSquare(unsigned i, int x, int y) {
			try {
				if (i >= CVSquares::Squares.size()) return false;

				const auto& sq = CVSquares::Squares[i];

				if (x >= sq.TL.x && x <= sq.BR.x && y >= sq.TL.y && y <= sq.BR.y) {
					return true;
				}

				return false;
			}
			catch (...) {
				return false;
			}
		}
	}

	void MotionDetection::detectMovement() {
		while (true) {
			try {
				const size_t count = (CVSquares::Squares.size() < CVSquares::Frames.size()) ? CVSquares::Squares.size() : CVSquares::Frames.size();

				const int MOTION_ON_STABLE_MS = 30;
				const int MOTION_OFF_STABLE_MS = 10;
				static std::unordered_map<int, std::chrono::steady_clock::time_point> motionOnStart;
				static std::unordered_map<int, std::chrono::steady_clock::time_point> motionOffStart;
				static std::unordered_map<int, std::chrono::steady_clock::time_point> pressStartTimes;
				const int TAP_THRESHOLD_MS = 120;

				for (size_t i = 0; i < count; ++i) {
					try {
						auto& sq = CVSquares::Squares[i];
						auto& fr = CVSquares::Frames[i];

						if (fr.imgDil.empty()) {
							sq.MOTION_DETECTED = false;
							if (sq.KEY != 0) {
								int vk = normalize_vk(sq.KEY);
								if (!(DetectionValues::executingJump && DetectionValues::currentDirectionKey == vk)) {
									MapKeys::ReleaseKey(vk);
								}
							}
							continue;
						}
					}
					catch (...) {
						// per-square exception
					}
				}
			}
			catch (...) {
				// global exception
			}
		}
	}
}
