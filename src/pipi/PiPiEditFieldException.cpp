#include "PiPiEditFieldException.h"

namespace PiPi
{
  const map<const PiPiEditFieldException::PiPiEditFieldExceptionCode, const string> PiPiEditFieldException::PiPiEditFieldExceptionMessageCodeMap{
      {PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable, "InOperable"},
      {PiPiEditFieldException::PiPiEditFieldExceptionCode::NotImplementate, "Not implementate"},
      {PiPiEditFieldException::PiPiEditFieldExceptionCode::MultilineNotSupported, "Multiline only support with textbox form field"},
      {PiPiEditFieldException::PiPiEditFieldExceptionCode::NotRegisterFont, "Specific font not registered"},
      {PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor, "Invalid color number, color number should between 0 to 1"}};

  PiPiEditFieldException::PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode code) : runtime_error(PiPiEditFieldException::PiPiEditFieldExceptionMessageCodeMap.at(code))
  {
    this->code = code;
  }

  const PiPiEditFieldException::PiPiEditFieldExceptionCode PiPiEditFieldException::getCode()
  {
    return this->code;
  }
}
