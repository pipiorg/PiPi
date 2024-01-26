#include "PiPiFontRegisterException.h"

namespace PiPi
{
  const map<const PiPiFontRegisterException::PiPiFontRegisterExceptionCode, const string> PiPiFontRegisterException::PiPiFontRegisterExceptionMessageCodeMap{
      {PiPiFontRegisterException::PiPiFontRegisterExceptionCode::InOperable, "InOperable"}};

  PiPiFontRegisterException::PiPiFontRegisterException(PiPiFontRegisterException::PiPiFontRegisterExceptionCode code) : runtime_error(PiPiFontRegisterException::PiPiFontRegisterExceptionMessageCodeMap.at(code))
  {
    this->code = code;
  }

  const PiPiFontRegisterException::PiPiFontRegisterExceptionCode PiPiFontRegisterException::getCode()
  {
    return this->code;
  }
}
