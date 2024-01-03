#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiPageUtil {
        public:
            static int SearchPageIndex(PdfMemDocument* document, PdfPage* page);
    };
}
