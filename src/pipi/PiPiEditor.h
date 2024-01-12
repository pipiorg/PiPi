#pragma once

#include <iostream>
#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiStringCommon.h"
#include "PiPiAppearanceManager.h"
#include "PiPiFontManager.h"
#include "PiPiFieldManager.h"
#include "PiPiFieldStyleManager.h"
#include "PiPiEditFieldException.h"
#include "PiPiTextHorizontalAlignment.h"

using namespace PoDoFo;

namespace PiPi
{
  class PiPiEditor
  {
    friend class PiPiOperator;

  public:
    PiPiEditor(PdfMemDocument *document, PiPiFontManager *fontManager, PiPiAppearanceManager *appearanceManager, PiPiFieldManager *fieldManager, PiPiFieldStyleManager *fieldStyleManager);

    PiPiEditor *Flatten();
    PiPiEditor *Flatten(std::string fieldName);

    PiPiEditor *AddField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);

    PiPiEditor *RemoveField(std::string fieldName);
    PiPiEditor *RemoveField(std::string fieldName, long pageIndex);
    PiPiEditor *RemoveField(std::string fieldName, long pageIndex, double x, double y);
    PiPiEditor *RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height);

    PiPiEditor *RenameField(std::string oldFieldName, std::string newFieldName);

    PiPiEditor *SetFieldFontName(std::string fieldName, std::string fontName);
    PiPiEditor *SetFieldFontName(std::string fieldName, long pageIndex, std::string fontName);
    PiPiEditor *SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, std::string fontName);
    PiPiEditor *SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string fontName);

    PiPiEditor *SetFieldFontSize(std::string fieldName, float fontSize);
    PiPiEditor *SetFieldFontSize(std::string fieldName, long pageIndex, float fontSize);
    PiPiEditor *SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, float fontSize);
    PiPiEditor *SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, double width, double height, float fontSize);

    PiPiEditor *SetFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment);
    PiPiEditor *SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, PiPiTextHorizontalAlignment alignment);
    PiPiEditor *SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, PiPiTextHorizontalAlignment alignment);
    PiPiEditor *SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, double width, double height, PiPiTextHorizontalAlignment alignment);

    PiPiEditor *SetFieldColor(std::string fieldName, float red, float green, float blue);
    PiPiEditor *SetFieldColor(std::string fieldName, long pageIndex, float red, float green, float blue);
    PiPiEditor *SetFieldColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue);
    PiPiEditor *SetFieldColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue);

    PiPiEditor *SetFieldBackgroundColor(std::string fieldName, float red, float green, float blue);
    PiPiEditor *SetFieldBackgroundColor(std::string fieldName, long pageIndex, float red, float green, float blue);
    PiPiEditor *SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue);
    PiPiEditor *SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue);

    PiPiEditor *SetFieldBorderColor(std::string fieldName, float red, float green, float blue);
    PiPiEditor *SetFieldBorderColor(std::string fieldName, long pageIndex, float red, float green, float blue);
    PiPiEditor *SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue);
    PiPiEditor *SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue);

    PiPiEditor* SetFieldBorderWidth(std::string fieldName, double borderWidth);
    PiPiEditor* SetFieldBorderWidth(std::string fieldName, long pageIndex, double borderWidth);
    PiPiEditor* SetFieldBorderWidth(std::string fieldName, long pageIndex, double x, double y, double borderWidth);
    PiPiEditor* SetFieldBorderWidth(std::string fieldName, long pageIndex, double x, double y, double width, double height, double borderWidth);

    PiPiEditor *SetFieldMultiline(std::string fieldName, bool multiline);
    PiPiEditor *SetFieldMultiline(std::string fieldName, long pageIndex, bool multiline);
    PiPiEditor *SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, bool multiline);
    PiPiEditor *SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, double width, double height, bool multiline);

    PiPiEditor *SetFieldDefaultValue(std::string fieldName, std::string defaultValue);
    PiPiEditor *SetFieldDefaultValue(std::string fieldName, long pageIndex, std::string defaultValue);
    PiPiEditor *SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, std::string defaultValue);
    PiPiEditor *SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string defaultValue);

    bool IsOperable();

  private:
    bool operable;

    PdfMemDocument *document;

    PiPiFontManager *fontManager;
    PiPiAppearanceManager *appearanceManager;
    PiPiFieldManager *fieldManager;
    PiPiFieldStyleManager *fieldStyleManager;
  };
}
