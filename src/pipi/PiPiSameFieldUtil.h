#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>

using namespace PoDoFo;

namespace PiPi
{
  class PiPiSameFieldUtil
  {
  public:
    static void ExpandField(PdfMemDocument* document, PdfObject* fieldObj);
    static void RestrictField(PdfMemDocument* document, PdfObject* fieldObj);
  };
}
