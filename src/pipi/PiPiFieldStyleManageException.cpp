#include "PiPiFieldStyleManageException.h"

namespace PiPi
{
  const map<const PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode, const string> PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionMessageCodeMap{
      {PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::NotRegisterFont, "Specific font not registered"},
      {PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor, "Invalid color number, color number should between 0 to 1"},
      {PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::MultilineNotSupported, "Multiline only support with textbox form field"}};

  PiPiFieldStyleManageException::PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode code) : runtime_error(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionMessageCodeMap.at(code))
  {
    this->code = code;
  }

  const PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode PiPiFieldStyleManageException::getCode()
  {
    return this->code;
  }
}
