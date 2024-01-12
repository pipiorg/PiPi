#include "PiPiExtractException.h"

namespace PiPi
{
  const map<const PiPiExtractException::PiPiExtractExceptionCode, const string> PiPiExtractException::PiPiExtractExceptionMessageCodeMap{
      {PiPiExtractException::PiPiExtractExceptionCode::InOperable, "InOperable"}};

  PiPiExtractException::PiPiExtractException(PiPiExtractException::PiPiExtractExceptionCode code) : runtime_error(PiPiExtractException::PiPiExtractExceptionMessageCodeMap.at(code))
  {
    this->code = code;
  }

  const PiPiExtractException::PiPiExtractExceptionCode PiPiExtractException::getCode()
  {
    return this->code;
  }
}
