#include "PiPiFontManageException.h"

namespace PiPi {
    const map<const PiPiFontManageException::PiPiFontManageExceptionCode, const string> PiPiFontManageException::PiPiFontManageExceptionMessageCodeMap {
        { PiPiFontManageException::PiPiFontManageExceptionCode::InOperable, "InOperable"}
    };

    PiPiFontManageException::PiPiFontManageException(PiPiFontManageException::PiPiFontManageExceptionCode code) : runtime_error(PiPiFontManageException::PiPiFontManageExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiFontManageException::PiPiFontManageExceptionCode PiPiFontManageException::getCode() {
        return this->code;
    }
}
