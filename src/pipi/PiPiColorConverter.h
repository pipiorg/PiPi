#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiColorConverter {
        public:
            static void ConvertGrayToRGB (float gray, float* red, float* green, float* blue);
            static void ConvertRGBToGray (float red, float green, float blue, float* gray);
            static void ConvertCMYKToRGB (float cyan, float magenta, float yellow, float black, float* red, float* green, float* blue);
            static void ConvertRGBToCMYK (float red, float green, float blue, float* cyan, float* megenta, float* yellow, float* black);
            static void ConvertPoDoFoArrayToRGB (PdfArray* colors, float* red, float* green, float* blue);
            static void ConvertRGBToPoDoFoArray (float red, float green, float blue, PdfArray** colors);
    };
}
