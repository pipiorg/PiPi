#include "PiPiEditFieldException.h"

namespace PiPi {
    const map<const PiPiEditFieldException::PiPiEditFieldExceptionCode, const string> PiPiEditFieldException::PiPiEditFieldExceptionMessageCodeMap {
        { PiPiEditFieldException::PiPiEditFieldExceptionCode::NotImplementate, "Not implementate" },
        { PiPiEditFieldException::PiPiEditFieldExceptionCode::MultilineNotSupported, "Multiline only support with textbox form field" },
        { PiPiEditFieldException::PiPiEditFieldExceptionCode::NotRegisterFont, "Specific font not registered" }
    };

    PiPiEditFieldException::PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode code) : runtime_error(PiPiEditFieldException::PiPiEditFieldExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiEditFieldException::PiPiEditFieldExceptionCode PiPiEditFieldException::getCode() {
        return this->code;
    }
}
