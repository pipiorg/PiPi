#include "PiPiFieldCompatibilityException.h"

namespace PiPi {
    const map<const PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionCode, const string> PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionMessageCodeMap {
        { PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionCode::FixDotFieldFail, "Invalid pdf with invalid dot field" }
    };

    PiPiFieldCompatibilityException::PiPiFieldCompatibilityException(PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionCode code) : runtime_error(PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionCode PiPiFieldCompatibilityException::getCode() {
        return this->code;
    }
}
