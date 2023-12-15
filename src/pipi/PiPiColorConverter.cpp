#include "PiPiColorConverter.h"

namespace PiPi {
    void PiPiColorConverter::ConvertGrayToRGB(float gray, float *red, float *green, float *blue) {
        *red = gray;
        *green = gray;
        *blue = gray;
    }

    void PiPiColorConverter::ConvertRGBToGray(float red, float green, float blue, float *gray) {
        *gray =  red * 0.299 + green * 0.587 + blue * 0.114;
    }

    void PiPiColorConverter::ConvertCMYKToRGB(float cyan, float magenta, float yellow, float black, float *red, float *green, float *blue) {
        *red = 1.0 - std::min(1.0, (cyan * (1.0 - black)) + black);
        *green = 1.0 - std::min(1.0, (magenta * (1.0 - black)) + black);
        *blue = 1.0 - std::min(1.0, (yellow * (1.0 - black)) + black);
    }

    void PiPiColorConverter::ConvertRGBToCMYK(float red, float green, float blue, float *cyan, float *megenta, float *yellow, float *black) {
        *black = 1.0 - std::max(std::max(red, green), blue);
        *cyan = (1.0 - red - *black) / (1.0 - *black);
        *megenta = (1.0 - green - *black) / (1 - *black);
        *yellow = (1.0 - blue - *black) / (1 - *black);
    }

    void PiPiColorConverter::ConvertPoDoFoArrayToRGB(PdfArray *colors, float *red, float *green, float *blue) {
        size_t colorCount = colors->size();
        
        if (colorCount == 1) {
            float gray = (float)colors->GetAtAs<double>(0);
            
            ConvertGrayToRGB(gray, red, green, blue);
        } else if (colorCount == 3) {
            *red = (float)colors->GetAtAs<double>(0);
            *green = (float)colors->GetAtAs<double>(1);
            *blue = (float)colors->GetAtAs<double>(2);
        } else if (colorCount == 4) {
            float cyan = (float)colors->GetAtAs<double>(0);
            float magenta = (float)colors->GetAtAs<double>(1);
            float yellow = (float)colors->GetAtAs<double>(2);
            float black = (float)colors->GetAtAs<double>(3);
            
            ConvertCMYKToRGB(cyan, magenta, yellow, black, red, green, blue);
        }
    }
}
