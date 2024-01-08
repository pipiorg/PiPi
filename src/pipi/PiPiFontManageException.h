#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi {
    class PiPiFontManageException : public runtime_error {
        public:
            enum PiPiFontManageExceptionCode {
                InOperable = 1
            };
        
            PiPiFontManageException(PiPiFontManageExceptionCode code);
    
            const PiPiFontManageExceptionCode getCode();
        private:
            static const map<const PiPiFontManageExceptionCode, const string> PiPiFontManageExceptionMessageCodeMap;
        
            PiPiFontManageExceptionCode code;
    };
}
