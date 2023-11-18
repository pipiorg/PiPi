#include "PiPiCommon.h"

namespace PiPi {
    std::vector<std::string>* PiPiCommon::split(const std::string& str, const std::string& pattern) {
        std::vector<std::string>* result = new std::vector<std::string>();
        std::string::size_type begin, end;

        end = str.find(pattern);
        begin = 0;

        while (end != std::string::npos) {
            if (end - begin != 0) {
                result->push_back(str.substr(begin, end - begin));
            }

            begin = end + pattern.size();
            end = str.find(pattern, begin);
        }

        if (begin != str.length()) {
            result->push_back(str.substr(begin));
        }

        return result;
    }

    std::string PiPiCommon::join(const std::vector<std::string>* strings, std::string gap) {
        std::string result = "";

        for (auto iterator = strings->begin(); iterator != strings->end() - 1; iterator.operator++()) {
            std::string string = *iterator;
            result.append(string);
            result.append(gap);
        }

        std::string string = strings->back();
        result.append(string);

        return result;
    }
}