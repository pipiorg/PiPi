#include "PiPiManageFieldException.h"

namespace PiPi {
    const map<const PiPiManageFieldException::PiPiManageFieldExceptionCode, const string> PiPiManageFieldException::PiPiManageFieldExceptionMessageCodeMap {
        { PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPdfFieldObject, "Cannot create a pdf field from a invalid pdf object" },
        { PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPdfFieldType, "Unsupported specific pdf field type" },
        { PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidRestrictFieldOperation, "Invalid restrict field operation" },
        { PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidRealStatus, "Invalid real status" },
        { PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPageIndex, "Invalid page index" },
        { PiPiManageFieldException::PiPiManageFieldExceptionCode::DuplicateFieldExists, "Duplicate field already exists" },
        { PiPiManageFieldException::PiPiManageFieldExceptionCode::UnsupportRemoveFakeField, "Unsupported remove fake field" }
    };

    PiPiManageFieldException::PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode code) : runtime_error(PiPiManageFieldException::PiPiManageFieldExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiManageFieldException::PiPiManageFieldExceptionCode PiPiManageFieldException::getCode() {
        return this->code;
    }
}
