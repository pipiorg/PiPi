#include "PiPiColorConverter.h"

namespace PiPi
{
  void PiPiColorConverter::ConvertGrayToRGB(float gray, float *red, float *green, float *blue)
  {
    spdlog::trace("ConvertGrayToRGB");

    *red = gray;
    *green = gray;
    *blue = gray;
  }

  void PiPiColorConverter::ConvertRGBToGray(float red, float green, float blue, float *gray)
  {
    spdlog::trace("ConvertRGBToGray");

    *gray = red * 0.299f + green * 0.587f + blue * 0.114f;
  }

  void PiPiColorConverter::ConvertCMYKToRGB(float cyan, float magenta, float yellow, float black, float *red, float *green, float *blue)
  {
    spdlog::trace("ConvertCMYKToRGB");

    *red = 1.0f - std::min(1.0f, (cyan * (1.0f - black)) + black);
    *green = 1.0f - std::min(1.0f, (magenta * (1.0f - black)) + black);
    *blue = 1.0f - std::min(1.0f, (yellow * (1.0f - black)) + black);
  }

  void PiPiColorConverter::ConvertRGBToCMYK(float red, float green, float blue, float *cyan, float *megenta, float *yellow, float *black)
  {
    spdlog::trace("ConvertRGBToCMYK");

    *black = 1.0f - std::max(std::max(red, green), blue);
    *cyan = (1.0f - red - *black) / (1.0f - *black);
    *megenta = (1.0f - green - *black) / (1.0f - *black);
    *yellow = (1.0f - blue - *black) / (1.0f - *black);
  }

  void PiPiColorConverter::ConvertPoDoFoArrayToRGB(PdfArray *colors, float *red, float *green, float *blue)
  {
    spdlog::trace("ConvertPoDoFoArrayToRGB");

    size_t colorCount = colors->size();

    if (colorCount == 1)
    {
      float gray = (float)colors->GetAtAs<double>(0);

      ConvertGrayToRGB(gray, red, green, blue);
    }
    else if (colorCount == 3)
    {
      *red = (float)colors->GetAtAs<double>(0);
      *green = (float)colors->GetAtAs<double>(1);
      *blue = (float)colors->GetAtAs<double>(2);
    }
    else if (colorCount == 4)
    {
      float cyan = (float)colors->GetAtAs<double>(0);
      float magenta = (float)colors->GetAtAs<double>(1);
      float yellow = (float)colors->GetAtAs<double>(2);
      float black = (float)colors->GetAtAs<double>(3);

      ConvertCMYKToRGB(cyan, magenta, yellow, black, red, green, blue);
    }
  }

  void PiPiColorConverter::ConvertRGBToPoDoFoArray(float red, float green, float blue, PdfArray **colors)
  {
    spdlog::trace("ConvertRGBToPoDoFoArray");

    *colors = new PdfArray();

    (*colors)->Add(PdfObject(red));
    (*colors)->Add(PdfObject(green));
    (*colors)->Add(PdfObject(blue));
  }
}
