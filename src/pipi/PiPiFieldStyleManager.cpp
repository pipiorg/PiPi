#include "PiPiFieldStyleManager.h"

namespace PiPi
{
  PiPiFieldStyleManager::PiPiFieldStyleManager(PdfMemDocument *document, PiPiFontManager *fontManager, PiPiFieldManager *fieldManager)
  {
    this->document = document;
    this->fontManager = fontManager;
    this->fieldManager = fieldManager;
  }

  void PiPiFieldStyleManager::SetFieldFontName(std::string fieldName, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    this->SetFieldFontName(fieldName, -1, fontName);
  }

  void PiPiFieldStyleManager::SetFieldFontName(std::string fieldName, long pageIndex, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    this->SetFieldFontName(fieldName, pageIndex, -1, -1, fontName);
  }

  void PiPiFieldStyleManager::SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    this->SetFieldFontName(fieldName, pageIndex, x, y, -1, -1, fontName);
  }

  void PiPiFieldStyleManager::SetFieldFontName(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string fontName)
  {
    spdlog::trace("SetFieldFontName");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldFontName(fieldObj, fontName);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldFontSize(std::string fieldName, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    this->SetFieldFontSize(fieldName, -1, fontSize);
  }

  void PiPiFieldStyleManager::SetFieldFontSize(std::string fieldName, long pageIndex, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    this->SetFieldFontSize(fieldName, pageIndex, -1, -1, fontSize);
  }

  void PiPiFieldStyleManager::SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    this->SetFieldFontSize(fieldName, pageIndex, x, y, -1, -1, fontSize);
  }

  void PiPiFieldStyleManager::SetFieldFontSize(std::string fieldName, long pageIndex, double x, double y, double width, double height, float fontSize)
  {
    spdlog::trace("SetFieldFontSize");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldFontSize(fieldObj, fontSize);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    this->SetFieldTextHorizontalAlignment(fieldName, -1, alignment);
  }

  void PiPiFieldStyleManager::SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    this->SetFieldTextHorizontalAlignment(fieldName, pageIndex, -1, -1, alignment);
  }

  void PiPiFieldStyleManager::SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    this->SetFieldTextHorizontalAlignment(fieldName, pageIndex, x, y, -1, -1, alignment);
  }

  void PiPiFieldStyleManager::SetFieldTextHorizontalAlignment(std::string fieldName, long pageIndex, double x, double y, double width, double height, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("SetFieldTextHorizontalAlignment");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldTextHorizontalAlignment(fieldObj, alignment);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldColor(std::string fieldName, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    this->SetFieldColor(fieldName, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldColor(std::string fieldName, long pageIndex, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    this->SetFieldColor(fieldName, pageIndex, -1, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    this->SetFieldColor(fieldName, pageIndex, x, y, -1, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue)
  {
    spdlog::trace("SetFieldColor");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldColor(fieldObj, red, green, blue);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldBackgroundColor(std::string fieldName, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    this->SetFieldBackgroundColor(fieldName, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldBackgroundColor(std::string fieldName, long pageIndex, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    this->SetFieldBackgroundColor(fieldName, pageIndex, -1, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    this->SetFieldBackgroundColor(fieldName, pageIndex, x, y, -1, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldBackgroundColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBackgroundColor");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldBackgroundColor(fieldObj, red, green, blue);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldBorderColor(std::string fieldName, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    this->SetFieldBorderColor(fieldName, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldBorderColor(std::string fieldName, long pageIndex, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    this->SetFieldBorderColor(fieldName, pageIndex, -1, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    this->SetFieldBorderColor(fieldName, pageIndex, x, y, -1, -1, red, green, blue);
  }

  void PiPiFieldStyleManager::SetFieldBorderColor(std::string fieldName, long pageIndex, double x, double y, double width, double height, float red, float green, float blue)
  {
    spdlog::trace("SetFieldBorderColor");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldBorderColor(fieldObj, red, green, blue);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldBorderWidth(std::string fieldName, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    this->SetFieldBorderWidth(fieldName, -1, borderWidth);
  }

  void PiPiFieldStyleManager::SetFieldBorderWidth(std::string fieldName, long pageIndex, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    this->SetFieldBorderWidth(fieldName, pageIndex, -1, -1, borderWidth);
  }

  void PiPiFieldStyleManager::SetFieldBorderWidth(std::string fieldName, long pageIndex, double x, double y, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    this->SetFieldBorderWidth(fieldName, pageIndex, x, y, -1, -1, borderWidth);
  }

  void PiPiFieldStyleManager::SetFieldBorderWidth(std::string fieldName, long pageIndex, double x, double y, double width, double height, double borderWidth)
  {
    spdlog::trace("SetFieldBorderWidth");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldBorderWidth(fieldObj, borderWidth);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldMultiline(std::string fieldName, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    this->SetFieldMultiline(fieldName, -1, multiline);
  }

  void PiPiFieldStyleManager::SetFieldMultiline(std::string fieldName, long pageIndex, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    this->SetFieldMultiline(fieldName, pageIndex, -1, -1, multiline);
  }

  void PiPiFieldStyleManager::SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    this->SetFieldMultiline(fieldName, pageIndex, x, y, -1, -1, multiline);
  }

  void PiPiFieldStyleManager::SetFieldMultiline(std::string fieldName, long pageIndex, double x, double y, double width, double height, bool multiline)
  {
    spdlog::trace("SetFieldMultiline");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        this->InnerSetFieldMultiline(field, multiline);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::SetFieldDefaultValue(std::string fieldName, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    this->SetFieldDefaultValue(fieldName, -1, defaultValue);
  }

  void PiPiFieldStyleManager::SetFieldDefaultValue(std::string fieldName, long pageIndex, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    this->SetFieldDefaultValue(fieldName, pageIndex, -1, -1, defaultValue);
  }

  void PiPiFieldStyleManager::SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    this->SetFieldDefaultValue(fieldName, pageIndex, x, y, -1, -1, defaultValue);
  }

  void PiPiFieldStyleManager::SetFieldDefaultValue(std::string fieldName, long pageIndex, double x, double y, double width, double height, std::string defaultValue)
  {
    spdlog::trace("SetFieldDefaultValue");

    PiPiFieldManager *fieldManager = this->fieldManager;

    std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);

    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerSetFieldDefaultValue(fieldObj, defaultValue);
      }
    }

    delete fields;
  }

  void PiPiFieldStyleManager::InnerSetFieldFontName(PdfObject *obj, std::string fontName)
  {
    spdlog::trace("InnerSetFieldFontName");

    const PdfFont *font = fontManager->AccessFont(fontName);
    if (font == nullptr)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::NotRegisterFont);
    }

    const PdfString *defaultDA = this->GetDefaultDA();
    std::string defaultDAString = defaultDA->GetString();

    PdfDictionary *dict = &(obj->GetDictionary());

    PdfDictionary *parentDict = dict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    PdfObject *daObj = dict->FindKey(PdfName("DA"));
    while (daObj == nullptr && parentDict != nullptr)
    {
      daObj = parentDict->FindKey(PdfName("DA"));
      parentDict = parentDict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    }

    std::string daString = daObj == nullptr
                               ? defaultDAString
                               : daObj->GetString().GetString();

    std::vector<std::string> *daStringSplits = PiPiStringCommon::split(daString, " ");
    std::vector<std::string> *newDaStringSplits = new std::vector<std::string>();

    for (int i = 0; i < daStringSplits->size(); i++)
    {
      if (i == 0)
      {
        newDaStringSplits->push_back("/" + fontName);
        continue;
      }

      std::string daStringSplit = (*daStringSplits)[i];
      newDaStringSplits->push_back(daStringSplit);
    }

    std::string newDaString = PiPiStringCommon::join(newDaStringSplits, " ");

    dict->RemoveKey(PdfName("DA"));
    dict->AddKey(PdfName("DA"), PdfString(newDaString));

    delete newDaStringSplits;
    delete daStringSplits;
  }

  void PiPiFieldStyleManager::InnerSetFieldFontSize(PdfObject *obj, float fontSize)
  {
    spdlog::trace("InnerSetFieldFontSize");

    const PdfString *defaultDA = this->GetDefaultDA();
    std::string defaultDAString = defaultDA->GetString();

    PdfDictionary *dict = &(obj->GetDictionary());

    PdfDictionary *parentDict = dict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    PdfObject *daObj = dict->FindKey(PdfName("DA"));
    while (daObj == nullptr && parentDict != nullptr)
    {
      daObj = parentDict->FindKey(PdfName("DA"));
      parentDict = parentDict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    }

    std::string daString = daObj == nullptr
                               ? defaultDAString
                               : daObj->GetString().GetString();

    std::vector<std::string> *daStringSplits = PiPiStringCommon::split(daString, " ");
    std::vector<std::string> *newDaStringSplits = new std::vector<std::string>();

    for (int i = 0; i < daStringSplits->size(); i++)
    {
      if (i == 1)
      {
        newDaStringSplits->push_back(std::to_string((int)fontSize));
        continue;
      }

      std::string daStringSplit = (*daStringSplits)[i];
      newDaStringSplits->push_back(daStringSplit);
    }

    std::string newDaString = PiPiStringCommon::join(newDaStringSplits, " ");

    dict->RemoveKey(PdfName("DA"));
    dict->AddKey(PdfName("DA"), PdfString(newDaString));

    delete newDaStringSplits;
    delete daStringSplits;
  }

  void PiPiFieldStyleManager::InnerSetFieldTextHorizontalAlignment(PdfObject *obj, PiPiTextHorizontalAlignment alignment)
  {
    spdlog::trace("InnerSetFieldTextHorizontalAlignment");

    PdfDictionary *fieldDict = &(obj->GetDictionary());

    if (fieldDict->HasKey(PdfName("Q")))
    {
      fieldDict->RemoveKey(PdfName("Q"));
    }

    switch (alignment)
    {
    case PiPiTextHorizontalAlignment::Center:
      fieldDict->AddKey(PdfName("Q"), PdfObject((int64_t)1));
      break;
    case PiPiTextHorizontalAlignment::Right:
      fieldDict->AddKey(PdfName("Q"), PdfObject((int64_t)2));
      break;
    case PiPiTextHorizontalAlignment::Left:
    default:
      fieldDict->AddKey(PdfName("Q"), PdfObject((int64_t)0));
      break;
    }
  }

  void PiPiFieldStyleManager::InnerSetFieldColor(PdfObject *obj, float red, float green, float blue)
  {
    spdlog::trace("InnerSetFieldColor");

    if (red < 0 || red > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    if (green < 0 || green > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    if (blue < 0 || blue > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    const PdfString *defaultDA = this->GetDefaultDA();
    std::string defaultDAString = defaultDA->GetString();

    PdfDictionary *dict = &(obj->GetDictionary());

    PdfDictionary *parentDict = dict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    PdfObject *daObj = dict->FindKey(PdfName("DA"));
    while (daObj == nullptr && parentDict != nullptr)
    {
      daObj = parentDict->FindKey(PdfName("DA"));
      parentDict = parentDict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    }

    std::string daString = daObj == nullptr
                               ? defaultDAString
                               : daObj->GetString().GetString();

    std::vector<std::string> *daStringSplits = PiPiStringCommon::split(daString, " ");
    std::vector<std::string> *newDaStringSplits = new std::vector<std::string>();

    std::string colorSign = daStringSplits->back();

    size_t keepCount = daStringSplits->size();

    if (colorSign == "g")
    {
      keepCount -= 2;
    }
    else if (colorSign == "rg")
    {
      keepCount -= 4;
    }
    else if (colorSign == "k")
    {
      keepCount -= 5;
    }

    for (size_t i = 0; i < keepCount; i++)
    {
      newDaStringSplits->push_back(daStringSplits->at(i));
    }

    newDaStringSplits->push_back(std::to_string(red));
    newDaStringSplits->push_back(std::to_string(green));
    newDaStringSplits->push_back(std::to_string(blue));
    newDaStringSplits->push_back("rg");

    std::string newDaString = PiPiStringCommon::join(newDaStringSplits, " ");

    dict->RemoveKey(PdfName("DA"));
    dict->AddKey(PdfName("DA"), PdfString(newDaString));

    delete daStringSplits;
    delete newDaStringSplits;
  }

  void PiPiFieldStyleManager::InnerSetFieldBackgroundColor(PdfObject *obj, float red, float green, float blue)
  {
    spdlog::trace("InnerSetFieldBackgroundColor");

    if (red < 0 || red > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    if (green < 0 || green > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    if (blue < 0 || blue > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    PdfDictionary *fieldDict = &(obj->GetDictionary());

    PdfObject *fieldMkObj = fieldDict->FindKey(PdfName("MK"));
    if (fieldMkObj == nullptr)
    {
      fieldDict->AddKey(PdfName("MK"), PdfDictionary());
      fieldMkObj = fieldDict->FindKey(PdfName("MK"));
    }

    PdfDictionary *fieldMk = &(fieldMkObj->GetDictionary());

    PdfObject *fieldBgObj = fieldMk->FindKey(PdfName("BG"));
    if (fieldBgObj != nullptr)
    {
      fieldMk->RemoveKey(PdfName("BG"));
    }

    PdfArray *colors;
    PiPiColorConverter::ConvertRGBToPoDoFoArray(red, green, blue, &colors);

    fieldMk->AddKey(PdfName("BG"), *colors);
  }

  void PiPiFieldStyleManager::InnerSetFieldBorderColor(PdfObject *obj, float red, float green, float blue)
  {
    spdlog::trace("InnerSetFieldBorderColor");

    if (red < 0 || red > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    if (green < 0 || green > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    if (blue < 0 || blue > 1)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::InvalidColor);
    }

    PdfDictionary *fieldDict = &(obj->GetDictionary());

    PdfObject *fieldMkObj = fieldDict->FindKey(PdfName("MK"));
    if (fieldMkObj == nullptr)
    {
      fieldDict->AddKey(PdfName("MK"), PdfDictionary());
      fieldMkObj = fieldDict->FindKey(PdfName("MK"));
    }

    PdfDictionary *fieldMk = &(fieldMkObj->GetDictionary());

    PdfObject *fieldBcObj = fieldMk->FindKey(PdfName("BC"));
    if (fieldBcObj != nullptr)
    {
      fieldMk->RemoveKey(PdfName("BC"));
    }

    PdfArray *colors;
    PiPiColorConverter::ConvertRGBToPoDoFoArray(red, green, blue, &colors);

    fieldMk->AddKey(PdfName("BC"), *colors);
  }

  void PiPiFieldStyleManager::InnerSetFieldBorderWidth(PdfObject *obj, double borderWidth)
  {
    spdlog::trace("InnerSetFieldBorderWidth");

    if (borderWidth <= 0)
      return;

    PdfDictionary *dict = &(obj->GetDictionary());

    PdfObject *bsObj = dict->FindKey(PdfName("BS"));
    if (bsObj == nullptr)
    {
      dict->AddKey(PdfName("BS"), PdfDictionary());
      bsObj = dict->FindKey(PdfName("BS"));
    }

    PdfDictionary *bs = &(bsObj->GetDictionary());

    if (bs->HasKey(PdfName("W")))
    {
      bs->RemoveKey(PdfName("W"));
    }

    bs->AddKey(PdfName("W"), PdfObject(borderWidth));
  }

  void PiPiFieldStyleManager::InnerSetFieldMultiline(PdfField *field, bool multiline)
  {
    spdlog::trace("InnerSetFieldMultiline");

    PdfFieldType type = field->GetType();
    if (type != PdfFieldType::TextBox)
    {
      throw PiPiFieldStyleManageException(PiPiFieldStyleManageException::PiPiFieldStyleManageExceptionCode::MultilineNotSupported);
    }

    PdfTextBox *textbox = (PdfTextBox *)field;
    textbox->SetMultiLine(multiline);
  }

  void PiPiFieldStyleManager::InnerSetFieldDefaultValue(PdfObject *obj, std::string defaultValue)
  {
    spdlog::trace("InnerSetFieldDefaultValue");

    PdfDictionary *fieldDict = &(obj->GetDictionary());

    if (fieldDict->HasKey(PdfName("DV")))
    {
      fieldDict->RemoveKey(PdfName("DV"));
    }

    fieldDict->AddKey(PdfName("DV"), PdfString(defaultValue));
  }

  const PdfString *PiPiFieldStyleManager::GetDefaultDA()
  {
    spdlog::trace("GetDefaultDA");

    PdfDocument *document = this->document;

    PdfAcroForm *acroform = document->GetAcroForm();
    PdfDictionary *acroformDict = &(acroform->GetDictionary());

    PdfObject *acroformDAObj = acroformDict->FindKey(PdfName("DA"));
    const PdfString *acroformDA = &(acroformDAObj->GetString());

    return acroformDA;
  }
}
