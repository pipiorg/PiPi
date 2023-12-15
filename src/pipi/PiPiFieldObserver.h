#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldObserver {
        public:
            PiPiFieldObserver();

        private:
            std::map<const std::string, const PdfField*>* fieldMap;
    };
}
