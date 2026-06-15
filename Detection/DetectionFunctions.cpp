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
}
