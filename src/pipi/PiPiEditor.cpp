#include "PiPiEditor.h"

namespace PiPi
{
  PiPiEditor::PiPiEditor(PdfMemDocument *document, PiPiFontManager *fontManager, PiPiAppearanceManager *appearanceManager, PiPiFieldManager *fieldManager, PiPiFieldStyleManager *fieldStyleManager)
  {
    this->operable = true;
    this->document = document;
    this->fontManager = fontManager;
    this->appearanceManager = appearanceManager;
    this->fieldManager = fieldManager;
    this->fieldStyleManager = fieldStyleManager;
  }

  bool PiPiEditor::IsOperable()
  {
    return this->operable;
  }

  PiPiEditor *PiPiEditor::Flatten()
  {
    spdlog::trace("Flatten");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PdfMemDocument *document = this->document;
    PiPiFontManager *fontManager = this->fontManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldManager *fieldManager = this->fieldManager;

    std::map<const std::string, std::set<PdfField *> *> *fieldMap = fieldManager->SearchAllField();

    for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++())
    {
      std::set<PdfField *> *fields = mapIterator->second;

      for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
      {
        PdfField *field = *iterator;
        PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

        PiPiAppearanceUtil::FlattenAppearance(fontManager, fieldManager, annot);
      }
    }

    delete fieldMap;

    fieldManager->RemoveAllField();
    appearanceManager->ClearNeedAppearance();

    return this;
  }

  PiPiEditor *PiPiEditor::Flatten(std::string fieldName)
  {
    spdlog::trace("Flatten");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PdfMemDocument *document = this->document;
    PiPiFontManager *fontManager = this->fontManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PiPiAppearanceUtil::FlattenAppearance(fontManager, fieldManager, annot);
    }

    delete fields;

    fieldManager->RemoveField(fieldName);
    appearanceManager->UnMarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::AddField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height)
  {
    spdlog::trace("AddField");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PdfMemDocument *document = this->document;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldManager *fieldManager = this->fieldManager;

    fieldManager->CreateField(fieldName, type, pageIndex, x, y, width, height);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::RemoveField(std::string fieldName)
  {
    spdlog::trace("RemoveField");

    return this->RemoveField(fieldName, -1);
  }

  PiPiEditor *PiPiEditor::RemoveField(std::string fieldName, long pageIndex)
  {
    spdlog::trace("RemoveField");

    return this->RemoveField(fieldName, pageIndex, -1, -1);
  }

  PiPiEditor *PiPiEditor::RemoveField(std::string fieldName, long pageIndex, double x, double y)
  {
    spdlog::trace("RemoveField");

    return this->RemoveField(fieldName, pageIndex, x, y, -1, -1);
  }

  PiPiEditor *PiPiEditor::RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height)
  {
    spdlog::trace("RemoveField");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PdfMemDocument *document = this->document;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldManager *fieldManager = this->fieldManager;

    fieldManager->RemoveField(fieldName, pageIndex, x, y, width, height);
    appearanceManager->UnMarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::RenameField(std::string oldFieldName, std::string newFieldName)
  {
    spdlog::trace("RenameField");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PdfMemDocument *document = this->document;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldManager *fieldManager = this->fieldManager;

    fieldManager->RenameField(oldFieldName, newFieldName);

    appearanceManager->UnMarkNeedAppearance(oldFieldName);
    appearanceManager->MarkNeedAppearance(newFieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldColor(std::string fieldName, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldColor(fieldName, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldColor(std::string fieldName, long pageIndex, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldColor(fieldName, pageIndex, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldColor(fieldName, pageIndex, x, y, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldColor(fieldName, pageIndex, x, y, width, height, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBorderColor(std::string fieldName, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderColor(fieldName, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBorderColor(std::string fieldName, long pageIndex, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderColor(fieldName, pageIndex, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderColor(fieldName, pageIndex, x, y, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderColor(fieldName, pageIndex, x, y, width, height, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor* PiPiEditor::SetFieldBorderWidth(std::string fieldName, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager* appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager* fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderWidth(fieldName, borderWidth);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor* PiPiEditor::SetFieldBorderWidth(std::string fieldName, long pageIndex, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager* appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager* fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderWidth(fieldName, pageIndex, borderWidth);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor* PiPiEditor::SetFieldBorderWidth(std::string fieldName, long pageIndex, double x, double y, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager* appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager* fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderWidth(fieldName, pageIndex, x, y, borderWidth);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;

  }

  PiPiEditor* PiPiEditor::SetFieldBorderWidth(std::string fieldName, long pageIndex, double x, double y, double width, double height, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager* appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager* fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldBorderWidth(fieldName, pageIndex, x, y, width, height, borderWidth);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBackgroundColor(std::string fieldName, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldBackgroundColor(fieldName, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBackgroundColor(std::string fieldName, long pageIndex, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldBackgroundColor(fieldName, pageIndex, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldBackgroundColor(fieldName, pageIndex, x, y, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldBackgroundColor(fieldName, pageIndex, x, y, width, height, red, green, blue);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldTextHorizontalAlignment(fieldName, alignment);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldTextHorizontalAlignment(fieldName, pageIndex, alignment);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldTextHorizontalAlignment(fieldName, pageIndex, x, y, alignment);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, double width, double height, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldTextHorizontalAlignment(fieldName, pageIndex, x, y, width, height, alignment);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldMultiline(std::string fieldName, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldMultiline(fieldName, multiline);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldMultiline(std::string fieldName, long pageIndex, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldMultiline(fieldName, pageIndex, multiline);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldMultiline(fieldName, pageIndex, x, y, multiline);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, double width, double height, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiAppearanceManager *appearanceManager = this->appearanceManager;
    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;

    fieldStyleManager->SetFieldMultiline(fieldName, pageIndex, x, y, width, height, multiline);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontName(std::string fieldName, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontName(fieldName, fontName);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontName(std::string fieldName, long pageIndex, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontName(fieldName, pageIndex, fontName);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontName(fieldName, pageIndex, x, y, fontName);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontName(fieldName, pageIndex, x, y, width, height, fontName);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontSize(std::string fieldName, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontSize(fieldName, fontSize);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontSize(std::string fieldName, long pageIndex, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontSize(fieldName, pageIndex, fontSize);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontSize(fieldName, pageIndex, x, y, fontSize);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, double width, double height, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldFontSize(fieldName, pageIndex, x, y, width, height, fontSize);
    appearanceManager->MarkNeedAppearance(fieldName);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldDefaultValue(std::string fieldName, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldDefaultValue(fieldName, defaultValue);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldDefaultValue(std::string fieldName, long pageIndex, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldDefaultValue(fieldName, pageIndex, defaultValue);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldDefaultValue(fieldName, pageIndex, x, y, defaultValue);

    return this;
  }

  PiPiEditor *PiPiEditor::SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    if (!this->IsOperable())
    {
      throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
    }

    PiPiFieldStyleManager *fieldStyleManager = this->fieldStyleManager;
    PiPiAppearanceManager *appearanceManager = this->appearanceManager;

    fieldStyleManager->SetFieldDefaultValue(fieldName, pageIndex, x, y, width, height, defaultValue);

    return this;
  }
}
