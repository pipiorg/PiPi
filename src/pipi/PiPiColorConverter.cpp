#include "PiPiColorConverter.h"

namespace PiPi {
    void PiPiColorConverter::ConvertPoDoFoArrayToRGB(PdfArray *colors, float *red, float *green, float *blue) {
        size_t colorCount = colors->size();
        
        if (colorCount == 1) {
            float color = (float)colors->GetAtAs<double>(0);
            
            *red = color;
            *green = color;
            *blue = color;
        } else if (colorCount == 3) {
            *red = (float)colors->GetAtAs<double>(0);
            *green = (float)colors->GetAtAs<double>(1);
            *blue = (float)colors->GetAtAs<double>(2);
        } else if (colorCount == 4) {
            float cyan = (float)colors->GetAtAs<double>(0);
            float magenta = (float)colors->GetAtAs<double>(1);
            float yellow = (float)colors->GetAtAs<double>(2);
            float black = (float)colors->GetAtAs<double>(3);
            
            *red = 1.0 - std::min(1.0, (cyan * (1.0 - black)) + black);
            *green = 1.0 - std::min(1.0, (magenta * (1.0 - black)) + black);
            *blue = 1.0 - std::min(1.0, (yellow * (1.0 - black)) + black);
        }
    }
}
