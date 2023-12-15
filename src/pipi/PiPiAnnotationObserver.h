#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiAnnotationObserver {
        public:
            PiPiAnnotationObserver();
            ~PiPiAnnotationObserver();
        
        private:
            std::map<const std::string, const PdfAnnotation*>* annotMap;
    };
}
