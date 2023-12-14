#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiColorConverter {
        public:
            static void ConvertPoDoFoArrayToRGB (PdfArray* colors, float* red, float* green, float* blue);
    };
}
