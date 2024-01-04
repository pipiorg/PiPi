#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiPageUtil {
        public:
            static int SearchPageIndex(PdfMemDocument* document, PdfPage* page);
            
            static double ExtractPageWidth(PdfPage* page);
            static double ExtractPageHeight(PdfPage* page);

            static double ExtractPageX(PdfPage* page);
            static double ExtractPageY(PdfPage* page);
    };
}
