#include "PiPiAppearanceException.h"

namespace PiPi {
    const map<const PiPiAppearanceException::PiPiAppearanceExceptionCode, const string> PiPiAppearanceException::PiPiAppearanceExceptionMessageCodeMap {
        {}
    };

    PiPiAppearanceException::PiPiAppearanceException(PiPiAppearanceException::PiPiAppearanceExceptionCode code) : runtime_error(PiPiAppearanceException::PiPiAppearanceExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiAppearanceException::PiPiAppearanceExceptionCode PiPiAppearanceException::getCode() {
        return this->code;
    }
}
