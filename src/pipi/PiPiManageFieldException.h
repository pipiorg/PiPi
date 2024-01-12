#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiManageFieldException : public runtime_error
  {
  public:
    enum PiPiManageFieldExceptionCode
    {
      InvalidPdfFieldObject = 1,
      InvalidPdfFieldType = 2,
      InvalidRestrictFieldOperation = 3,
      InvalidRealStatus = 4,
      InvalidPageIndex = 5,
      DuplicateFieldExists = 6,
      UnsupportRemoveFakeField = 7,
      UnsupportRenameFakeField = 8
    };

    PiPiManageFieldException(PiPiManageFieldExceptionCode code);

    const PiPiManageFieldExceptionCode getCode();

  private:
    static const map<const PiPiManageFieldExceptionCode, const string> PiPiManageFieldExceptionMessageCodeMap;

    PiPiManageFieldExceptionCode code;
  };
}
