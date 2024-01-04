#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi {
    class PiPiPageException : public runtime_error {
        public:
            enum PiPiPageExceptionCode {
                InvalidSplitInstruction = 0,
                IndexOutOfRange = 1
            };
        
            PiPiPageException(PiPiPageExceptionCode code);
    
            const PiPiPageExceptionCode getCode();
        private:
            static const map<const PiPiPageExceptionCode, const string> PiPiPageExceptionMessageCodeMap;
        
            PiPiPageExceptionCode code;
    };
}
