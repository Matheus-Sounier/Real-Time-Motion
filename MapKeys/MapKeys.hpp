#pragma once

#include <unordered_map>
#include <Windows.h>
#include <string>

namespace MapKeys {

	extern std::unordered_map<char, int> keyMap;

	extern std::unordered_map<int, int> keyMapEX;

	extern std::unordered_map<int, std::string> keyEXName;

	void PressKey(int key);

	void ReleaseKey(int key);
}