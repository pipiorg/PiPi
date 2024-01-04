#pragma once

#include "podofo/podofo.h"
#include "PiPiStringCommon.h"
#include "PiPiFieldConstants.h"
#include "PiPiFieldCompatibilityException.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldCompatibilityUtil {
        public:
            static void Patch(PdfMemDocument* document);
        
            static void PatchFieldDot(PdfMemDocument* document);
            static void PatchFieldRestrict(PdfMemDocument* document);
        private:
            static std::vector<PdfObject*>* CollectDotField(PdfDocument* document);
            static void CollectDotFieldRecursive(PdfDocument* document, PdfObject* fieldObj, std::vector<PdfObject*>* dotFieldObjs);
        
            static std::vector<PdfObject*>* CollectRestrictField(PdfDocument* document);
            static void CollectRestrictFieldRecursive(PdfDocument* document, PdfObject* fieldObj, std::vector<PdfObject*>* restrictFieldObjs);
        
            static void FixRestrict(PdfDocument* document, PdfObject* fieldObj);
            static void FixExpand(PdfDocument* document, PdfObject* fieldObj);
        
            static void FixDot(PdfDocument* document, PdfObject* fieldObj);
            static void FixDotParentField(PdfDocument* document, PdfObject* fieldObj);
            static void FixDotAcroform(PdfDocument* document, PdfObject* fieldObj);
        
            static PdfObject* PrepareFieldParentField(PdfDocument* document, PdfObject* parentFieldObj, std::string name);
            static PdfObject* PrepareFieldAcroform(PdfDocument* document, std::string name);
        
            static bool IsRoot(PdfObject* fieldObj);
            static bool IsReal(PdfObject* fieldObj);
            static bool IsNoBrother(PdfObject* fieldObj);
        
    };
}
