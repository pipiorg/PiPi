#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldCompatibilityUtil {
        public:
            static void Patch(PdfMemDocument* document);
        
            static void PatchFieldDot(PdfMemDocument* document);
            static void PatchFieldRestrict(PdfMemDocument* document);
        private:
            static std::vector<PdfObject*>* CollectRestrictField(PdfDocument* document);
            static void CollectRestrictFieldRecursive(PdfDocument* document, PdfObject* fieldObj, std::vector<PdfObject*>* restrictFieldObjs);
        
            static void Restrict(PdfDocument* document, PdfObject* fieldObj);
        
            static bool IsNoBrother(PdfObject* fieldObj);
        
    };
}
