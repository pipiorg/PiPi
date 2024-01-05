#include "PiPiMultiOperateException.h"

namespace PiPi {
    const map<const PiPiMultiOperateException::PiPiMultiOperateExceptionCode, const string> PiPiMultiOperateException::PiPiMultiOperateExceptionMessageCodeMap {
        { PiPiMultiOperateException::PiPiMultiOperateExceptionCode::IndexOutOfRange, "Index out of range" }
    };

    PiPiMultiOperateException::PiPiMultiOperateException(PiPiMultiOperateException::PiPiMultiOperateExceptionCode code) : runtime_error(PiPiMultiOperateException::PiPiMultiOperateExceptionMessageCodeMap.at(code)) {
        this->code = code;
    }

    const PiPiMultiOperateException::PiPiMultiOperateExceptionCode PiPiMultiOperateException::getCode() {
        return this->code;
    }
}
