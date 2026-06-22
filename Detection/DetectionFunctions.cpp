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
				// motion loop
				const size_t count = (CVSquares::Squares.size() < CVSquares::Frames.size()) ? CVSquares::Squares.size() : CVSquares::Frames.size();

				const int MOTION_ON_STABLE_MS = 30; // Duration doing moviments in the area
				const int MOTION_OFF_STABLE_MS = 10; // Duration to release the key
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

						int changed = 0;
						try {
							if (!fr.imgThres.empty()) {
								std::vector<std::vector<cv::Point>> contours;
								cv::Mat tmp = fr.imgThres.clone();
								cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
								for (const auto& c : contours) {
									double a = cv::contourArea(c);
									if (a >= GraphicsValues::TOLERANCE) changed += static_cast<int>(a);
								}
							}
						}
						catch (...) { changed = 0; }

						int computed = DetectionValues::MOVEMENT_THRESHOLD / 6;
						int hysteresis = (computed > 1) ? computed : 1;
						int offThreshold = DetectionValues::MOVEMENT_THRESHOLD - hysteresis;
						bool motion = sq.MOTION_DETECTED ? (changed >= offThreshold) : (changed >= DetectionValues::MOVEMENT_THRESHOLD);

						int sid = sq.ID;
						auto now = std::chrono::steady_clock::now();

						if (motion) {
							if (!sq.MOTION_DETECTED) {
								auto it = motionOnStart.find(sid);
								if (it == motionOnStart.end()) motionOnStart[sid] = now;
								else {
									auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
									if (elapsed >= MOTION_ON_STABLE_MS) {
										sq.MOTION_DETECTED = true;
										sq.COLOR = cv::Scalar(0, 255, 0);
										if (sq.KEY != 0 && !(DetectionValues::jumpStored || DetectionValues::executingJump)) MapKeys::PressKey(normalize_vk(sq.KEY));
										pressStartTimes[sid] = now;
										motionOffStart.erase(sid);
										motionOnStart.erase(sid);
									}
								}
							}
							else {
								motionOffStart.erase(sid);
								motionOnStart.erase(sid);
							}
						}
						else {
							if (sq.MOTION_DETECTED) {
								auto itPress = pressStartTimes.find(sid);
								if (itPress != pressStartTimes.end()) {
									auto pressDur = std::chrono::duration_cast<std::chrono::milliseconds>(now - itPress->second).count();
									if (pressDur < TAP_THRESHOLD_MS) {
										sq.MOTION_DETECTED = false;
										sq.COLOR = cv::Scalar(0, 0, 255);
										if (sq.KEY != 0) {
											int vk = normalize_vk(sq.KEY);
											if (!(DetectionValues::executingJump && DetectionValues::currentDirectionKey == vk) &&
												!(DetectionValues::jumpStored && DetectionValues::currentDirectionKey == vk)) {
												MapKeys::ReleaseKey(vk);
											}
										}
										motionOnStart.erase(sid);
										motionOffStart.erase(sid);
										pressStartTimes.erase(sid);
										continue;
									}
								}
								auto it = motionOffStart.find(sid);
								if (it == motionOffStart.end()) motionOffStart[sid] = now;
								else {
									auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
									if (elapsed >= MOTION_OFF_STABLE_MS) {
										sq.MOTION_DETECTED = false;
										sq.COLOR = cv::Scalar(0, 0, 255);
										if (sq.KEY != 0) {
											int vk = normalize_vk(sq.KEY);
											if (!(DetectionValues::executingJump && DetectionValues::currentDirectionKey == vk) &&
												!(DetectionValues::jumpStored && DetectionValues::currentDirectionKey == vk)) {
												MapKeys::ReleaseKey(vk);
											}
										}
										motionOnStart.erase(sid);
										motionOffStart.erase(sid);
										pressStartTimes.erase(sid);
									}
								}
							}
							else {
								motionOnStart.erase(sid);
								motionOffStart.erase(sid);
							}
						}
					}
					catch (...) {
						// swallow per-square exceptions
					}
				}
			}
			catch (...) {
				// swallow loop exceptions
			}

			std::this_thread::sleep_for(15ms);
		}
	}

	void MotionDetection::detectJump() {
		static std::chrono::steady_clock::time_point lastLandingTime;
		while (true) {
			try {
				if (!jumpDetectionActivated) {
					// Reset any jump-related flags and wait
					chargingJump = false;
					jumpStored = false;
					std::this_thread::sleep_for(50ms);
					continue;
				}


				// Need jump image available
				if (CVMatFrames::JumpIMGThres.empty() && CVMatFrames::JumpIMGDil.empty()) {
					std::this_thread::sleep_for(20ms);
					continue;
				}

				// Use dilated image if available, else threshold image
				cv::Mat imgToCheck = CVMatFrames::JumpIMGDil.empty() ? CVMatFrames::JumpIMGThres : CVMatFrames::JumpIMGDil;

				// Count changed area using contours filtered by tolerance for stability
				int changed = 0;
				try {
					if (!imgToCheck.empty()) {
						std::vector<std::vector<cv::Point>> contours;
						cv::Mat tmp = imgToCheck.clone();
						cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
						for (const auto& c : contours) {
							double a = cv::contourArea(c);
							if (a >= GraphicsValues::TOLERANCE) changed += static_cast<int>(a);
						}
					}
				}
				catch (...) { changed = 0; }

				const int JUMP_COOLDOWN_MS = 100;

				if (jumpStored && changed >= JUMP_THRESHOLD) {
					// ensure cooldown passed
					auto nowCheck = std::chrono::steady_clock::now();
					if (lastLandingTime.time_since_epoch().count() != 0) {
						auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(nowCheck - lastLandingTime).count();
						if (diff < JUMP_COOLDOWN_MS) {
							// ignore this spike as it's within the cooldown window
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

				try {
					for (size_t qi = 0; qi < CVSquares::Frames.size() && qi < CVSquares::Squares.size(); ++qi) {
						int sqChanged = 0;
						try {
							const auto& f = CVSquares::Frames[qi];
							if (!f.imgThres.empty()) {
								std::vector<std::vector<cv::Point>> contours;
								cv::Mat tmp = f.imgThres.clone();
								cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
								for (const auto& c : contours) {
									double a = cv::contourArea(c);
									if (a >= GraphicsValues::TOLERANCE) sqChanged += static_cast<int>(a);
								}
							}
						}
						catch (...) { sqChanged = 0; }
						// debug output removed to avoid printing square debug information
					}
					// no cv::waitKey here to avoid interfering with main UI
				}
				catch (...) {
					// ignore debug failures
				}
				const int PRE_STABLE_MS = 80; // require presence for this period before starting charge
				static std::chrono::steady_clock::time_point preStableStart;
				if (!chargingJump && changed >= JUMP_THRESHOLD) {
					auto now = std::chrono::steady_clock::now();
					if (preStableStart.time_since_epoch().count() == 0) preStableStart = now;
					if (std::chrono::duration_cast<std::chrono::milliseconds>(now - preStableStart).count() >= PRE_STABLE_MS) {
						chargingJump = true;
						jumpStored = false;
						jumpStartTime = std::chrono::steady_clock::now();
						DetectionValues::currentChargingMs = 0;
						preStableStart = std::chrono::steady_clock::time_point();
					}
				}
				else if (changed < JUMP_THRESHOLD) {
					preStableStart = std::chrono::steady_clock::time_point();
				}

				bool restartedCharging = false;
				if (chargingJump) {
					try {
						auto nowDraw = std::chrono::steady_clock::now();
						DetectionValues::currentChargingMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(nowDraw - jumpStartTime).count());
					}
					catch (...) {}
					if (changed < JUMP_THRESHOLD) {

						const int LAND_STABLE_MS = 60;
						auto landNow = std::chrono::steady_clock::now();
						int stableWaited = 0;
						const int POLL_MS = 20;
						while (stableWaited < LAND_STABLE_MS) {
							std::this_thread::sleep_for(std::chrono::milliseconds(POLL_MS));
							stableWaited += POLL_MS;
							// re-evaluate changed area
							int liveChanged = 0;
							try {
								cv::Mat liveImg = CVMatFrames::JumpIMGDil.empty() ? CVMatFrames::JumpIMGThres : CVMatFrames::JumpIMGDil;
								if (!liveImg.empty()) {
									std::vector<std::vector<cv::Point>> contours;
									cv::Mat tmp = liveImg.clone();
									cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
									for (const auto& c : contours) {
										double a = cv::contourArea(c);
										if (a >= GraphicsValues::TOLERANCE) liveChanged += static_cast<int>(a);
									}
								}
							}
							catch (...) { liveChanged = 0; }
							if (liveChanged >= JUMP_THRESHOLD) {
								// bounce back above threshold -> restart charging
								restartedCharging = true;
								break;
							}
						}

						if (restartedCharging) {
							chargingJump = true;
							// restart time
							jumpStartTime = std::chrono::steady_clock::now();
							DetectionValues::currentChargingMs = 0;
						}
						else {
							// Confirm landed
							chargingJump = false;
							auto now = std::chrono::steady_clock::now();
							auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - jumpStartTime).count();

							const int MIN_POWER = 50;
							const int MAX_POWER = 800;
							int power = static_cast<int>(duration);
							if (power < MIN_POWER) power = MIN_POWER;
							else if (power > MAX_POWER) power = MAX_POWER;

							DetectionValues::storedJumpPower = power;
							jumpStored = true;
							// record landing time to prevent immediate re-detection
							lastLandingTime = std::chrono::steady_clock::now();
						}

						bool directionChosen = false;
						int directionKey = 0; // virtual key code from MapKeys

						const int POLL_INTERVAL_MS = 30;
						const int STABLE_MS = 60;

						while (!directionChosen) {
							// scan squares for motion
							for (size_t si = 0; si < CVSquares::Squares.size(); ++si) {
								auto& sqRef = CVSquares::Squares[si];
								if (sqRef.MOTION_DETECTED && sqRef.KEY != 0) {
									// require stability
									auto t0 = std::chrono::steady_clock::now();
									int waited = 0;
									while (waited < STABLE_MS) {
										std::this_thread::sleep_for(std::chrono::milliseconds(POLL_INTERVAL_MS));
										waited += POLL_INTERVAL_MS;
										// re-check motion flag (it may have changed)
										if (!CVSquares::Squares[si].MOTION_DETECTED) break;
									}
									if (CVSquares::Squares[si].MOTION_DETECTED && waited >= STABLE_MS) {
										directionKey = CVSquares::Squares[si].KEY;
										directionChosen = true;
										break;
									}
								}
							}

							// If no direction chosen yet, also consider exit conditions: if jump detection disabled
							if (!jumpDetectionActivated) break;

							try {
								cv::Mat liveImg = CVMatFrames::JumpIMGDil.empty() ? CVMatFrames::JumpIMGThres : CVMatFrames::JumpIMGDil;
								int liveChanged = 0;
								try {
									if (!liveImg.empty()) {
										std::vector<std::vector<cv::Point>> contours;
										cv::Mat tmp = liveImg.clone();
										cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
										for (const auto& c : contours) {
											double a = cv::contourArea(c);
											if (a >= GraphicsValues::TOLERANCE) liveChanged += static_cast<int>(a);
										}
									}
								}
								catch (...) { liveChanged = 0; }
								if (liveChanged >= JUMP_THRESHOLD) {
									// begin new charging cycle reusing existing flags
									chargingJump = true;
									jumpStored = false;
									storedJumpPower = 0;
									jumpStartTime = std::chrono::steady_clock::now();
									DetectionValues::currentChargingMs = 0;
									restartedCharging = true;
									break;
								}
							}
							catch (...) {}

							std::this_thread::sleep_for(std::chrono::milliseconds(POLL_INTERVAL_MS));
						}

						if (restartedCharging) {
							// a new charging cycle started; go handle it in the outer loop so UI updates
							continue;
						}

						if (directionChosen && directionKey != 0) {
							// Normalize virtual-key codes
							int dirVK = normalize_vk(directionKey);
							DetectionValues::currentDirectionKey = dirVK;
							DetectionValues::executingJump = true;

							MapKeys::PressKey(dirVK);

							// Execute the stored jump: SPACE down, wait storedJumpPower, SPACE up
							int spaceVK = 0x20; // default
							auto itSpace = MapKeys::keyMapEX.find(32);
							if (itSpace != MapKeys::keyMapEX.end()) spaceVK = itSpace->second;

							if (spaceVK != 0) {
								MapKeys::PressKey(spaceVK);
								std::this_thread::sleep_for(std::chrono::milliseconds(storedJumpPower));
								MapKeys::ReleaseKey(spaceVK);
							}

							std::this_thread::sleep_for(std::chrono::milliseconds(5));

							// Release direction key after jump execution
							MapKeys::ReleaseKey(dirVK);
							DetectionValues::executingJump = false;
							DetectionValues::currentDirectionKey = 0;

							// Reset stored jump
							jumpStored = false;
							storedJumpPower = 0;
						}
						else {
							// No direction chosen; discard stored jump after a short timeout to avoid lingering
							const int WAIT_DISCARD_MS = 3000;
							int waited = 0;
							while (waited < WAIT_DISCARD_MS && !CVSquares::Squares.empty()) {
								bool anyMotion = false;
								for (const auto& sq : CVSquares::Squares) {
									if (sq.MOTION_DETECTED) {
										anyMotion = true; break;
									}
								}
								if (anyMotion) break;
								std::this_thread::sleep_for(std::chrono::milliseconds(40));
								waited += 100;
							}

							// discard
							jumpStored = false;
							storedJumpPower = 0;
						}
					}
					// else still above the line; just keep charging
				}

			}
			catch (...) {
				// swallow and continue
			}

			std::this_thread::sleep_for(15ms);
		}
	}
}
