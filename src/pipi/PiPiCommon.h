#pragma once

#include <iostream>
#include <vector>
#include <cstring>

namespace PiPi {
	class PiPiCommon {
		public:
			static std::vector<std::string>* split(const std::string& str, const std::string& pattern);
	};
}
