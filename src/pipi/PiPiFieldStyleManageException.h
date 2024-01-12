#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiFieldStyleManageException : public runtime_error
  {
  public:
    enum PiPiFieldStyleManageExceptionCode
    {
      NotRegisterFont = 1,
      InvalidColor = 2,
      MultilineNotSupported = 3
    };

    PiPiFieldStyleManageException(PiPiFieldStyleManageExceptionCode code);

    const PiPiFieldStyleManageExceptionCode getCode();

  private:
    static const map<const PiPiFieldStyleManageExceptionCode, const string> PiPiFieldStyleManageExceptionMessageCodeMap;

    PiPiFieldStyleManageExceptionCode code;
  };
}
