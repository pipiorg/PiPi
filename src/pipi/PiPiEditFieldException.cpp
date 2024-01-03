#include "PiPiEditFieldException.h"

namespace PiPi {
    const map<const PiPiEditFieldException::PiPiEditFieldExceptionCode, const string> PiPiEditFieldException::PiPiEditFieldExceptionMessageCodeMap {
        {}
    };

    PiPiEditFieldException::PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode code) : runtime_error(PiPiEditFieldException::PiPiEditFieldExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiEditFieldException::PiPiEditFieldExceptionCode PiPiEditFieldException::getCode() {
        return this->code;
    }
}
