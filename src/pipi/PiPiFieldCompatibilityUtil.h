#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiStringCommon.h"
#include "PiPiFieldConstants.h"
#include "PiPiFieldCompatibilityException.h"
#include "PiPiSameFieldUtil.h"

using namespace PoDoFo;

namespace PiPi
{
  class PiPiFieldCompatibilityUtil
  {
  public:
    static void Patch(PdfMemDocument *document);

    static void PatchFieldDot(PdfMemDocument *document);
    static void PatchFieldRestrict(PdfMemDocument *document);

  private:
    static std::vector<PdfObject *> *CollectDotField(PdfMemDocument*document);
    static void CollectDotFieldRecursive(PdfMemDocument*document, PdfObject *fieldObj, std::vector<PdfObject *> *dotFieldObjs);

    static std::vector<PdfObject *> *CollectRestrictField(PdfMemDocument*document);
    static void CollectRestrictFieldRecursive(PdfMemDocument*document, PdfObject *fieldObj, std::vector<PdfObject *> *restrictFieldObjs);

    static void FixRestrict(PdfMemDocument *document, PdfObject *fieldObj);

    static void FixDot(PdfMemDocument *document, PdfObject *fieldObj);
    static void FixDotParentField(PdfMemDocument*document, PdfObject *fieldObj);
    static void FixDotAcroform(PdfMemDocument *document, PdfObject *fieldObj);

    static PdfObject *PrepareFieldParentField(PdfMemDocument *document, PdfObject *parentFieldObj, std::string name);
    static PdfObject *PrepareFieldAcroform(PdfMemDocument *document, std::string name);

    static bool IsRoot(PdfObject *fieldObj);
    static bool IsReal(PdfObject *fieldObj);
    static bool IsNoBrother(PdfObject *fieldObj);
  };
}
