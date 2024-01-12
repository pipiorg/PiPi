#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
  class PiPiMultiOperateException : public runtime_error
  {
  public:
    enum PiPiMultiOperateExceptionCode
    {
      IndexOutOfRange = 1
    };

    PiPiMultiOperateException(PiPiMultiOperateExceptionCode code);

    const PiPiMultiOperateExceptionCode getCode();

  private:
    static const map<const PiPiMultiOperateExceptionCode, const string> PiPiMultiOperateExceptionMessageCodeMap;

    PiPiMultiOperateExceptionCode code;
  };
}
