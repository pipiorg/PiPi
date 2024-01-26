#include "PiPiAnnotationUtil.h"

namespace PiPi
{
  std::string PiPiAnnotationUtil::ExtractAnnotationFontName(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationFontName");

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *daObj = dict->FindKey(PdfName("DA"));
    PdfDictionary *parentDict = dict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));

    while (daObj == nullptr && parentDict != nullptr)
    {
      daObj = parentDict->FindKey(PdfName("DA"));
      parentDict = parentDict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    }

    if (daObj == nullptr)
    {
      return "";
    }

    if (!daObj->IsString())
    {
      return "";
    }

    PdfString da = daObj->GetString();
    std::string daValue = da.GetString();

    std::vector<std::string> *splitted = PiPiStringCommon::split(daValue, " ");
    size_t splitSize = splitted->size();

    if (splitSize >= 1)
    {
      std::string font = (*splitted)[0].substr(1);
      delete splitted;
      return font;
    }

    delete splitted;

    return "";
  }

  float PiPiAnnotationUtil::ExtractAnnotationFontSize(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationFontSize");

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *daObj = dict->FindKey(PdfName("DA"));
    PdfDictionary *parentDict = dict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));

    while (daObj == nullptr && parentDict != nullptr)
    {
      daObj = parentDict->FindKey(PdfName("DA"));
      parentDict = parentDict->FindKeyAs<PdfDictionary *>(PdfName("Parent"));
    }

    if (daObj == nullptr)
    {
      return 0.0;
    }

    if (!daObj->IsString())
    {
      return 0.0;
    }

    PdfString da = daObj->GetString();
    std::string daValue = da.GetString();

    std::vector<std::string> *splitted = PiPiStringCommon::split(daValue, " ");
    size_t splitSize = splitted->size();

    if (splitSize >= 2)
    {
      std::string fontSizeValue = (*splitted)[1];
      float fontSize = std::stof(fontSizeValue);
      delete splitted;
      return fontSize;
    }

    delete splitted;
    return 0.0;
  }

  double PiPiAnnotationUtil::ExtractAnnotationWidth(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationWidth");

    Rect rect = annotation->GetRect();

    double left = rect.GetLeft();
    double right = rect.GetRight();

    double width = right - left;

    return width;
  }

  double PiPiAnnotationUtil::ExtractAnnotationHeight(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationHeight");

    Rect rect = annotation->GetRect();

    double top = rect.GetTop();
    double bottom = rect.GetBottom();

    double height = top - bottom;

    return height;
  }

  double PiPiAnnotationUtil::ExtractAnnotationX(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationX");

    Rect rect = annotation->GetRect();

    double x = rect.GetLeft();

    return x;
  }

  double PiPiAnnotationUtil::ExtractAnnotationY(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationY");

    PdfPage *page = annotation->GetPage();

    Rect pageRect = page->GetRect();
    Rect annotRect = annotation->GetRect();

    double pageHeight = PiPiPageUtil::ExtractPageHeight(page);
    double annotHeight = ExtractAnnotationHeight(annotation);
    double y = pageHeight - annotRect.GetTop();

    return y;
  }

  bool PiPiAnnotationUtil::ExtractAnnotationBorderExists(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationBorderExists");

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *mkObj = dict->GetKey(PdfName("MK"));
    if (mkObj == nullptr)
    {
      return false;
    }

    if (!mkObj->IsDictionary())
    {
      return false;
    }

    PdfDictionary *mk = &(mkObj->GetDictionary());

    PdfObject *bcObj = mk->GetKey(PdfName("BC"));
    if (bcObj == nullptr)
    {
      return false;
    }

    if (!bcObj->IsArray())
    {
      return false;
    }

    return true;
  }

  double PiPiAnnotationUtil::ExtractAnnotationBorderWidth(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationBorderWidth");

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *bsObj = dict->FindKey(PdfName("BS"));
    if (bsObj == nullptr)
    {
      return 0.0;
    }

    PdfDictionary *bs = &(bsObj->GetDictionary());

    PdfObject *w = bs->FindKey(PdfName("W"));
    if (w == nullptr)
    {
      return 0.0;
    }

    if (w->IsNumber())
    {
      return (double)(w->GetNumber());
    }

    if (w->IsRealStrict())
    {
      return w->GetReal();
    }

    return 0.0;
  }

  void PiPiAnnotationUtil::ExtractAnnotationBorderColor(PdfAnnotation *annotation, float *red, float *green, float *blue)
  {
    spdlog::trace("ExtractAnnotationBorderColor");

    *red = 0;
    *green = 0;
    *blue = 0;

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *mkObj = dict->GetKey(PdfName("MK"));
    if (mkObj == nullptr)
    {
      return;
    }

    if (!mkObj->IsDictionary())
    {
      return;
    }

    PdfDictionary *mk = &(mkObj->GetDictionary());

    PdfObject *bcObj = mk->GetKey(PdfName("BC"));
    if (bcObj == nullptr)
    {
      return;
    }

    if (!bcObj->IsArray())
    {
      return;
    }

    PdfArray *bc = &(bcObj->GetArray());

    PiPiColorConverter::ConvertPoDoFoArrayToRGB(bc, red, green, blue);
  }

  void PiPiAnnotationUtil::ExtractAnnotationColor(PdfAnnotation *annotation, float *red, float *green, float *blue)
  {
    spdlog::trace("ExtractAnnotationColor");

    *red = 0;
    *green = 0;
    *blue = 0;

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *daObj = dict->FindKey("DA");

    if (daObj == nullptr)
    {
      return;
    }

    if (!daObj->IsString())
    {
      return;
    }

    PdfString da = daObj->GetString();
    std::string daValue = da.GetString();

    std::vector<std::string> *splitted = PiPiStringCommon::split(daValue, " ");
    size_t splitSize = splitted->size();

    if (splitSize <= 0)
    {
      return;
    }

    std::string colorSign = splitted->back();
    if (colorSign == "g")
    {
      if (splitSize >= 2)
      {
        float gray = std::stof((*splitted)[splitSize - 2]);

        PiPiColorConverter::ConvertGrayToRGB(gray, red, green, blue);
      }
    }
    else if (colorSign == "rg")
    {
      if (splitSize >= 4)
      {
        *red = std::stof((*splitted)[splitSize - 4]);
        *green = std::stof((*splitted)[splitSize - 3]);
        *blue = std::stof((*splitted)[splitSize - 2]);
      }
    }
    else if (colorSign == "k")
    {
      if (splitSize >= 5)
      {
        float cyan = std::stof((*splitted)[splitSize - 5]);
        float magenta = std::stof((*splitted)[splitSize - 4]);
        float yellow = std::stof((*splitted)[splitSize - 3]);
        float black = std::stof((*splitted)[splitSize - 2]);

        PiPiColorConverter::ConvertCMYKToRGB(cyan, magenta, yellow, black, red, green, blue);
      }
    }

    delete splitted;
  }

  bool PiPiAnnotationUtil::ExtractAnnotationBackgroundExists(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationBackgroundExists");

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *mkObj = dict->GetKey(PdfName("MK"));
    if (mkObj == nullptr)
    {
      return false;
    }

    if (!mkObj->IsDictionary())
    {
      return false;
    }

    PdfDictionary *mk = &(mkObj->GetDictionary());

    PdfObject *bcObj = mk->GetKey(PdfName("BG"));
    if (bcObj == nullptr)
    {
      return false;
    }

    if (!bcObj->IsArray())
    {
      return false;
    }

    return true;
  }

  void PiPiAnnotationUtil::ExtractAnnotationBackgroundColor(PdfAnnotation *annotation, float *red, float *green, float *blue)
  {
    spdlog::trace("ExtractAnnotationBackgroundColor");

    *red = 0;
    *green = 0;
    *blue = 0;

    PdfDictionary *dict = &(annotation->GetDictionary());

    PdfObject *mkObj = dict->GetKey(PdfName("MK"));
    if (mkObj == nullptr)
    {
      return;
    }

    if (!mkObj->IsDictionary())
    {
      return;
    }

    PdfDictionary *mk = &(mkObj->GetDictionary());

    PdfObject *bcObj = mk->GetKey(PdfName("BG"));
    if (bcObj == nullptr)
    {
      return;
    }

    if (!bcObj->IsArray())
    {
      return;
    }

    PdfArray *bc = &(bcObj->GetArray());

    PiPiColorConverter::ConvertPoDoFoArrayToRGB(bc, red, green, blue);
  }

  PiPiFieldType PiPiAnnotationUtil::ExtractAnnotationType(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationType");

    const PdfObject &constObject = annotation->GetObject();
    PdfObject &object = const_cast<PdfObject &>(constObject);

    std::unique_ptr<PdfField> fieldPtr;
    bool created = PdfField::TryCreateFromObject(object, fieldPtr);
    if (!created)
    {
      return PiPiFieldType::Unknown;
    }

    PdfField *field = fieldPtr.get();

    PdfFieldType type = field->GetType();

    switch (type)
    {
    case PdfFieldType::TextBox:
      fieldPtr.reset();
      return PiPiFieldType::TextBox;
    case PdfFieldType::CheckBox:
      fieldPtr.reset();
      return PiPiFieldType::CheckBox;
    default:
      fieldPtr.reset();
      return PiPiFieldType::Unknown;
    }
  }

  bool PiPiAnnotationUtil::ExtractAnnotationTextMultiine(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationTextMultiine");

    PiPiFieldType type = ExtractAnnotationType(annotation);
    if (type != PiPiFieldType::TextBox)
    {
      return false;
    }

    const PdfObject &constObject = annotation->GetObject();
    PdfObject &object = const_cast<PdfObject &>(constObject);

    std::unique_ptr<PdfField> fieldPtr;
    bool created = PdfField::TryCreateFromObject(object, fieldPtr);
    if (!created)
    {
      return false;
    }

    PdfField *field = fieldPtr.get();
    PdfTextBox *textbox = (PdfTextBox *)field;

    bool multiline = textbox->IsMultiLine();

    return multiline;
  }

  PiPiTextHorizontalAlignment PiPiAnnotationUtil::ExtractAnnotationTextHorizontalAlignment(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationTextHorizontalAlignment");

    PdfObject *obj = &(annotation->GetObject());

    PdfDictionary *dict = &(obj->GetDictionary());

    if (!dict->HasKey(PdfName("Q")))
    {
      return PiPiTextHorizontalAlignment::Left;
    }

    PdfObject *qObj = dict->FindKey(PdfName("Q"));
    int64_t alignmentValue = qObj->GetNumber();

    switch (alignmentValue)
    {
    case 1:
      return PiPiTextHorizontalAlignment::Center;
    case 2:
      return PiPiTextHorizontalAlignment::Right;
    default:
      return PiPiTextHorizontalAlignment::Left;
    }
  }

  std::string PiPiAnnotationUtil::ExtractAnnotationValue(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationValue");

    PdfObject *obj = &(annotation->GetObject());

    PdfDictionary *dict = &(obj->GetDictionary());

    PdfObject *vObj = dict->FindKey(PdfName("V"));
    if (vObj == nullptr)
    {
      return "";
    }

    if (!vObj->IsString())
    {
      return "";
    }

    const PdfString *v = &(vObj->GetString());

    return v->GetString();
  }

  std::string PiPiAnnotationUtil::ExtractAnnotationDefaultValue(PdfAnnotation *annotation)
  {
    spdlog::trace("ExtractAnnotationValue");

    PdfObject *obj = &(annotation->GetObject());

    PdfDictionary *dict = &(obj->GetDictionary());

    PdfObject *dvObj = dict->FindKey(PdfName("DV"));
    if (dvObj == nullptr)
    {
      return "";
    }

    if (!dvObj->IsString())
    {
      return "";
    }

    const PdfString *dv = &(dvObj->GetString());

    return dv->GetString();
  }
}
