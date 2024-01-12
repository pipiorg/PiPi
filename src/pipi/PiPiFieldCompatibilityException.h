#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiFieldCompatibilityException : public runtime_error
  {
  public:
    enum PiPiFieldCompatibilityExceptionCode
    {
      FixDotFieldFail = 0
    };

    PiPiFieldCompatibilityException(PiPiFieldCompatibilityExceptionCode code);

    const PiPiFieldCompatibilityExceptionCode getCode();

  private:
    static const map<const PiPiFieldCompatibilityExceptionCode, const string> PiPiFieldCompatibilityExceptionMessageCodeMap;

    PiPiFieldCompatibilityExceptionCode code;
  };
}
