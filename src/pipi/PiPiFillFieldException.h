#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi {
    class PiPiFillFieldException : public runtime_error {
        public:
            enum PiPiFillFieldExceptionCode {
                UnsupportedPdfFieldType = 1,
                
                
            };
        
            PiPiFillFieldException(PiPiFillFieldExceptionCode code);
    
            const PiPiFillFieldExceptionCode getCode();
        private:
            static const map<const PiPiFillFieldExceptionCode, const string> PiPiFillFieldExceptionMessageCodeMap;
        
            PiPiFillFieldExceptionCode code;
    };
}
