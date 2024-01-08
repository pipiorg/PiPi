#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <spdlog/spdlog.h>

namespace PiPi {
	class PiPiStringCommon {
		public:
			static std::vector<std::string>* split(const std::string& str, const std::string& pattern);
			static std::string join(const std::vector<std::string>* strings, std::string gap);
            static bool includes(const std::string& str, const std::string& substr);
            static bool startsWith(const std::string& str1, const std::string& str2);
	};
}
