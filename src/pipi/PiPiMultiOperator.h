#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiOperator.h"
#include "PiPiPager.h"
#include "PiPiStringCommon.h"
#include "PiPiMultiOperateException.h"

using namespace PoDoFo;

namespace PiPi
{
  class PiPiMultiOperator
  {
  public:
    PiPiMultiOperator();
    ~PiPiMultiOperator();

    size_t Add(char *pdfBytes, size_t pdfSize);

    PiPiOperator *GetOperator(size_t index);
    PiPiPager *GetPager();

  private:
    std::vector<std::tuple<PdfMemDocument *, PiPiOperator *>> *docs;

    PiPiPager *pager;
  };
}
