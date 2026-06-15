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
				// loop exception
			}

			std::this_thread::sleep_for(15ms);
		}
	}

	void MotionDetection::detectJump() {
		// State machine for jump detection and charging
		static std::chrono::steady_clock::time_point lastLandingTime;
		while (true) {
			try {
				if (!jumpDetectionActivated) {
					chargingJump = false;
					jumpStored = false;
					std::this_thread::sleep_for(50ms);
					continue;
				}

				if (CVMatFrames::JumpIMGThres.empty() && CVMatFrames::JumpIMGDil.empty()) {
					std::this_thread::sleep_for(20ms);
					continue;
				}

				cv::Mat imgToCheck = CVMatFrames::JumpIMGDil.empty() ? CVMatFrames::JumpIMGThres : CVMatFrames::JumpIMGDil;

				int changed = 0;
				try {
					if (!imgToCheck.empty()) {
						std::vector<std::vector<cv::Point>> contours;
						cv::Mat tmp = imgToCheck.clone();
						cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
						for (const auto &c : contours) {
							double a = cv::contourArea(c);
							if (a >= GraphicsValues::TOLERANCE) changed += static_cast<int>(a);
						}
					}
				} catch (...) { changed = 0; }

				const int JUMP_COOLDOWN_MS = 100;

				if (jumpStored && changed >= JUMP_THRESHOLD) {
					auto nowCheck = std::chrono::steady_clock::now();
					if (lastLandingTime.time_since_epoch().count() != 0) {
						auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(nowCheck - lastLandingTime).count();
						if (diff < JUMP_COOLDOWN_MS) {
							std::this_thread::sleep_for(15ms);
							continue;
						}
					}
					chargingJump = true;
					jumpStored = false;
					storedJumpPower = 0;
					jumpStartTime = std::chrono::steady_clock::now();
					DetectionValues::currentChargingMs = 0;
					std::this_thread::sleep_for(15ms);
					continue;
				}
			}
			catch (...) {
			}

			std::this_thread::sleep_for(15ms);
		}
	}
}
