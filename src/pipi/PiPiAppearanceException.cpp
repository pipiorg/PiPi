#include "PiPiAppearanceException.h"

namespace PiPi {
    const map<const PiPiAppearanceException::PiPiAppearanceExceptionCode, const string> PiPiAppearanceException::PiPiAppearanceExceptionMessageCodeMap {
        { PiPiAppearanceException::PiPiAppearanceExceptionCode::NoAppearanceExistsForFlatten, "Cannot flatten a field without appearance" },
        { PiPiAppearanceException::PiPiAppearanceExceptionCode::CreateFormObjectFromAppearanceFail, "Cannot create object from apperarance" },
        { PiPiAppearanceException::PiPiAppearanceExceptionCode::UnsupportedFieldType, "Unsupported field type" }
    };

    PiPiAppearanceException::PiPiAppearanceException(PiPiAppearanceException::PiPiAppearanceExceptionCode code) : runtime_error(PiPiAppearanceException::PiPiAppearanceExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiAppearanceException::PiPiAppearanceExceptionCode PiPiAppearanceException::getCode() {
        return this->code;
    }
}
