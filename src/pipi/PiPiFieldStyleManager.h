#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiFieldManager.h"
#include "PiPiFontManager.h"
#include "PiPiEditFieldException.h"


using namespace PoDoFo;

namespace PiPi
{
  class PiPiFieldStyleManager
  {
  public:
    PiPiFieldStyleManager(PdfMemDocument *document, PiPiFontManager *fontManager, PiPiFieldManager *fieldManager);

    void SetFieldFontName(std::string fieldName, std::string fontName);
    void SetFieldFontName(std::string fieldName, long pageIndex, std::string fontName);
    void SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, std::string fontName);
    void SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string fontName);

    void SetFieldFontSize(std::string fieldName, float fontSize);
    void SetFieldFontSize(std::string fieldName, long pageIndex, float fontSize);
    void SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, float fontSize);
    void SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, double width, double height, float fontSize);

    void SetFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment);
    void SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, PiPiTextHorizontalAlignment alignment);
    void SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, PiPiTextHorizontalAlignment alignment);
    void SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, double width, double height, PiPiTextHorizontalAlignment alignment);

    void SetFieldColor(std::string fieldName, float red, float green, float blue);
    void SetFieldColor(std::string fieldName, long pageIndex, float red, float green, float blue);
    void SetFieldColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue);
    void SetFieldColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue);

    void SetFieldBackgroundColor(std::string fieldName, float red, float green, float blue);
    void SetFieldBackgroundColor(std::string fieldName, long pageIndex, float red, float green, float blue);
    void SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue);
    void SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue);

    void SetFieldBorderColor(std::string fieldName, float red, float green, float blue);
    void SetFieldBorderColor(std::string fieldName, long pageIndex, float red, float green, float blue);
    void SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue);
    void SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue);

    void SetFieldMultiline(std::string fieldName, bool multiline);
    void SetFieldMultiline(std::string fieldName, long pageIndex, bool multiline);
    void SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, bool multiline);
    void SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, double width, double height, bool multiline);

    void SetFieldDefaultValue(std::string fieldName, std::string defaultValue);
    void SetFieldDefaultValue(std::string fieldName, long pageIndex, std::string defaultValue);
    void SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, std::string defaultValue);
    void SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string defaultValue);

  private:
    PdfMemDocument *document;
    PiPiFontManager *fontManager;
    PiPiFieldManager *fieldManager;

    void InnerSetFieldFontName(PdfObject *obj, std::string fontName);
    void InnerSetFieldFontSize(PdfObject *obj, float fontSize);
    void InnerSetFieldTextHorizontalAlignment(PdfObject *obj, PiPiTextHorizontalAlignment alignment);
    void InnerSetFieldColor(PdfObject *obj, float red, float green, float blue);
    void InnerSetFieldBackgroundColor(PdfObject *obj, float red, float green, float blue);
    void InnerSetFieldBorderColor(PdfObject *obj, float red, float green, float blue);
    void InnerSetFieldDefaultValue(PdfObject *obj, std::string defaultValue);
    void InnerSetFieldMultiline(PdfField *field, bool multiline);

    const PdfString *GetDefaultDA();
  };
}
