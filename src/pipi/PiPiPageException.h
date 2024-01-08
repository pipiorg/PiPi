#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi {
    class PiPiPageException : public runtime_error {
        public:
            enum PiPiPageExceptionCode {
                InOperable = 1,
                InvalidSplitInstruction = 2,
                IndexOutOfRange = 3
            };
        
            PiPiPageException(PiPiPageExceptionCode code);
    
            const PiPiPageExceptionCode getCode();
        private:
            static const map<const PiPiPageExceptionCode, const string> PiPiPageExceptionMessageCodeMap;
        
            PiPiPageExceptionCode code;
    };
}
