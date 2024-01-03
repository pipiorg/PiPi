#include "PiPiManageFieldException.h"

namespace PiPi {
    const map<const PiPiManageFieldException::PiPiManageFieldExceptionCode, const string> PiPiManageFieldException::PiPiManageFieldExceptionMessageCodeMap {
        { PiPiManageFieldExceptionCode::InvalidPdfFieldObject, "Cannot create a pdf field from a invalid pdf object" }
    };

    PiPiManageFieldException::PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode code) : runtime_error(PiPiManageFieldException::PiPiManageFieldExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiManageFieldException::PiPiManageFieldExceptionCode PiPiManageFieldException::getCode() {
        return this->code;
    }
}
