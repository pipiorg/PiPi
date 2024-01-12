#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiAppearanceException : public runtime_error
  {
  public:
    enum PiPiAppearanceExceptionCode
    {
      CreateFormObjectFromAppearanceFail = 0,
      UnsupportedFieldType = 1
    };

    PiPiAppearanceException(PiPiAppearanceExceptionCode code);

    const PiPiAppearanceExceptionCode getCode();

  private:
    static const map<const PiPiAppearanceExceptionCode, const string> PiPiAppearanceExceptionMessageCodeMap;

    PiPiAppearanceExceptionCode code;
  };
}
