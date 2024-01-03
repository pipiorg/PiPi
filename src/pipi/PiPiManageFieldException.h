#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi {
    class PiPiManageFieldException : public runtime_error {
        public:
            enum PiPiManageFieldExceptionCode {
                InvalidPdfFieldObject = 0,
                InvalidPdfFieldType = 1,
                InvalidRestrictFieldOperation = 2,
                InvalidRealStatus = 3,
                InvalidPageIndex = 4,
                DuplicateFieldExists = 5
                
                
            };
        
            PiPiManageFieldException(PiPiManageFieldExceptionCode code);
    
            const PiPiManageFieldExceptionCode getCode();
        private:
            static const map<const PiPiManageFieldExceptionCode, const string> PiPiManageFieldExceptionMessageCodeMap;
        
            PiPiManageFieldExceptionCode code;
    };
}
