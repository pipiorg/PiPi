#include "PiPiPageException.h"

namespace PiPi
{
  const map<const PiPiPageException::PiPiPageExceptionCode, const string> PiPiPageException::PiPiPageExceptionMessageCodeMap{
      {PiPiPageException::PiPiPageExceptionCode::InOperable, "InOperable"},
      {PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction, "Invalid split instruction"},
      {PiPiPageException::PiPiPageExceptionCode::IndexOutOfRange, "Index out of range"},
      {PiPiPageException::PiPiPageExceptionCode::MergeFieldConflict, "Merge field conflict"} };

  PiPiPageException::PiPiPageException(PiPiPageException::PiPiPageExceptionCode code) : runtime_error(PiPiPageException::PiPiPageExceptionMessageCodeMap.at(code))
  {
    this->code = code;
  }

  const PiPiPageException::PiPiPageExceptionCode PiPiPageException::getCode()
  {
    return this->code;
  }
}
