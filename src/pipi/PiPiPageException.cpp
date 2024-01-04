#include "PiPiPageException.h"

namespace PiPi {
    const map<const PiPiPageException::PiPiPageExceptionCode, const string> PiPiPageException::PiPiPageExceptionMessageCodeMap {
    };

    PiPiPageException::PiPiPageException(PiPiPageException::PiPiPageExceptionCode code) : runtime_error(PiPiPageException::PiPiPageExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiPageException::PiPiPageExceptionCode PiPiPageException::getCode() {
        return this->code;
    }
}
