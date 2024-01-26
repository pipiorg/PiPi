#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiFontRegisterException : public runtime_error
  {
  public:
    enum PiPiFontRegisterExceptionCode
    {
      InOperable = 1
    };

    PiPiFontRegisterException(PiPiFontRegisterExceptionCode code);

    const PiPiFontRegisterExceptionCode getCode();

  private:
    static const map<const PiPiFontRegisterExceptionCode, const string> PiPiFontRegisterExceptionMessageCodeMap;

    PiPiFontRegisterExceptionCode code;
  };
}
