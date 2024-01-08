#include "PiPiFillFieldException.h"

namespace PiPi {
    const map<const PiPiFillFieldException::PiPiFillFieldExceptionCode, const string> PiPiFillFieldException::PiPiFillFieldExceptionMessageCodeMap {
        { PiPiFillFieldException::PiPiFillFieldExceptionCode::InOperable, "Inoperable" },
        { PiPiFillFieldException::PiPiFillFieldExceptionCode::UnsupportedPdfFieldType, "Unsupported field type" }
    };

    PiPiFillFieldException::PiPiFillFieldException(PiPiFillFieldException::PiPiFillFieldExceptionCode code) : runtime_error(PiPiFillFieldException::PiPiFillFieldExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiFillFieldException::PiPiFillFieldExceptionCode PiPiFillFieldException::getCode() {
        return this->code;
    }
}
