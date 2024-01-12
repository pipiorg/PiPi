#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiExtractException : public runtime_error
  {
  public:
    enum PiPiExtractExceptionCode
    {
      InOperable = 1
    };

    PiPiExtractException(PiPiExtractExceptionCode code);

    const PiPiExtractExceptionCode getCode();

  private:
    static const map<const PiPiExtractExceptionCode, const string> PiPiExtractExceptionMessageCodeMap;

    PiPiExtractExceptionCode code;
  };
}
