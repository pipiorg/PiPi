#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldCompatibilityUtil {
        public:
            static void Patch(PdfMemDocument* document);
        
        static void PatchFieldFieldNameWithDot(PdfMemDocument* document);
        static void PatchFieldShouldRestrict(PdfMemDocument* document);
        
        static void InnerPatchFieldShouldRestrict(PdfObject* fieldObj);
    };
}
