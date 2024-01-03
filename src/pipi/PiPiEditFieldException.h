#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi {
    class PiPiEditFieldException : public runtime_error {
        public:
            enum PiPiEditFieldExceptionCode {
                NotImplementate = 1,
                MultilineNotSupported = 2,
                NotRegisterFont = 3
            };
        
            PiPiEditFieldException(PiPiEditFieldExceptionCode code);
    
            const PiPiEditFieldExceptionCode getCode();
        private:
            static const map<const PiPiEditFieldExceptionCode, const string> PiPiEditFieldExceptionMessageCodeMap;
        
            PiPiEditFieldExceptionCode code;
    };
}
