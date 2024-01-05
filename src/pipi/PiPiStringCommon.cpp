#include "PiPiStringCommon.h"

namespace PiPi {
    std::vector<std::string>* PiPiStringCommon::split(const std::string& str, const std::string& pattern) {
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

    std::string PiPiStringCommon::join(const std::vector<std::string>* strings, std::string gap) {
        std::string result = "";
        
        for (unsigned int idx = 0; idx < ((long)strings->size()) - 1; idx++) {
            std::string string = (*strings)[idx];
            result.append(string);
            result.append(gap);
        }

        std::string string = strings->back();
        result.append(string);

        return result;
    }

    bool PiPiStringCommon::includes(const std::string &str, const std::string &substr) {
        return str.find(substr) != std::string::npos;
    }

    bool PiPiStringCommon::startsWith(const std::string &str1, const std::string &str2) {
        return str1.rfind(str2, 0) == 0;
    }
}
