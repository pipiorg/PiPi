#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiOperator;

  class PiPiEditFieldException : public runtime_error
  {
  public:
    enum PiPiEditFieldExceptionCode
    {
      InOperable = 1,
      NotImplementate = 2
    };

    PiPiEditFieldException(PiPiEditFieldExceptionCode code);

    const PiPiEditFieldExceptionCode getCode();

  private:
    static const map<const PiPiEditFieldExceptionCode, const string> PiPiEditFieldExceptionMessageCodeMap;

    PiPiEditFieldExceptionCode code;
  };
}
