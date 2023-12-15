#include "PiPiExtractUtil.h"

namespace PiPi {
    double PiPiExtractUtil::ExtractPageWidth(PdfPage* page) {
        Rect rect = page->GetRect();

        double left = rect.GetLeft();
        double right = rect.GetRight();

        double width = right - left;

        return width;
    }

    double PiPiExtractUtil::ExtractPageHeight(PdfPage* page) {
        Rect rect = page->GetRect();

        double top = rect.GetTop();
        double bottom = rect.GetBottom();

        double height = top - bottom;

        return height;
    }

    double PiPiExtractUtil::ExtractPageX(PdfPage* page) {
        Rect rect = page->GetRect();

        double x = rect.GetLeft();

        return x;
    }

    double PiPiExtractUtil::ExtractPageY(PdfPage* page) {
        Rect rect = page->GetRect();

        double y = rect.GetTop();

        return y;
    }

    std::string PiPiExtractUtil::ExtractAnnotationName(PdfAnnotation *annotation) {
        PdfObject& objRef = annotation->GetObject();
        PdfObject* obj = &objRef;
        
        std::unique_ptr<PdfField> fieldPtr;
        bool created = PdfField::TryCreateFromObject(objRef, fieldPtr);
        if (!created) {
            return "";
        }

        PdfField* field = fieldPtr.get();

        PdfFieldType type = field->GetType();
        
        std::string name = field->GetFullName();
        
        return name;
    }

    std::string PiPiExtractUtil::ExtractAnnotationFontName(PdfAnnotation* annotation) {
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;

        PdfObject* daObj = dict->FindKey("DA");

        if (daObj == nullptr) {
            return "";
        }

        if (!daObj->IsString()) {
            return "";
        }

        PdfString da = daObj->GetString();
        std::string daValue = da.GetString();

        std::vector<std::string>* splitted = PiPiCommon::split(daValue, " ");
        size_t splitSize = splitted->size();

        if (splitSize >= 1) {
            std::string font = (*splitted)[0].substr(1);
            delete splitted;
            return font;
        }

        delete splitted;
        return "";
    }

    float PiPiExtractUtil::ExtractAnnotationFontSize(PdfAnnotation* annotation) {
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;

        PdfObject* daObj = dict->FindKey("DA");

        if (daObj == nullptr) {
            return 0;
        }

        if (!daObj->IsString()) {
            return 0;
        }

        PdfString da = daObj->GetString();
        std::string daValue = da.GetString();

        std::vector<std::string>* splitted = PiPiCommon::split(daValue, " ");
        size_t splitSize = splitted->size();

        if (splitSize >= 2) {
            std::string fontSizeValue = (*splitted)[1];
            float fontSize = std::stof(fontSizeValue);
            delete splitted;
            return fontSize;
        }

        delete splitted;
        return 0;
    }

    double PiPiExtractUtil::ExtractAnnotationWidth(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double left = rect.GetLeft();
        double right = rect.GetRight();

        double width = right - left;

        return width;
    }

    double PiPiExtractUtil::ExtractAnnotationHeight(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double top = rect.GetTop();
        double bottom = rect.GetBottom();

        double height = top - bottom;

        return height;
    }

    double PiPiExtractUtil::ExtractAnnotationX(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double y = rect.GetTop();

        return y;
    }

    double PiPiExtractUtil::ExtractAnnotationY(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double x = rect.GetLeft();

        return x;
    }

    bool PiPiExtractUtil::ExtractAnnotationBorderExists(PdfAnnotation *annotation) {
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;
        
        PdfObject* mkObj = dict->GetKey(PdfName("MK"));
        if (mkObj == nullptr) {
            return false;
        }
        
        if (!mkObj->IsDictionary()) {
            return false;
        }
        
        PdfDictionary& mkRef = mkObj->GetDictionary();
        PdfDictionary* mk = &mkRef;
        
        PdfObject* bcObj = mk->GetKey(PdfName("BC"));
        if (bcObj == nullptr) {
            return false;
        }
        
        if (!bcObj->IsArray()) {
            return false;
        }
        
        return true;
    }

    double PiPiExtractUtil::ExtractAnnotationBorderWidth(PdfAnnotation *annotation) {
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;
        
        PdfObject* bsObj = dict->FindKey(PdfName("BS"));
        if (bsObj == nullptr) {
            return 0;
        }
        
        PdfDictionary& bsRef = bsObj->GetDictionary();
        PdfDictionary* bs = &bsRef;
        
        PdfObject* w = bs->FindKey(PdfName("W"));
        if (w == nullptr) {
            return 0;
        }
        
        int width = (int) w->GetNumber();
        
        return width;
    }

    void PiPiExtractUtil::ExtractAnnotationBorderColor(PdfAnnotation *annotation, float *red, float *green, float *blue) {
        *red = 0;
        *green = 0;
        *blue = 0;
        
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;
        
        PdfObject* mkObj = dict->GetKey(PdfName("MK"));
        if (mkObj == nullptr) {
            return;
        }
        
        if (!mkObj->IsDictionary()) {
            return;
        }
        
        PdfDictionary& mkRef = mkObj->GetDictionary();
        PdfDictionary* mk = &mkRef;
        
        PdfObject* bcObj = mk->GetKey(PdfName("BC"));
        if (bcObj == nullptr) {
            return;
        }
        
        if (!bcObj->IsArray()) {
            return;
        }
        
        PdfArray& bcRef = bcObj->GetArray();
        PdfArray* bc = &bcRef;
        
        PiPiColorConverter::ConvertPoDoFoArrayToRGB(bc, red, green, blue);
    }

    void PiPiExtractUtil::ExtractAnnotationColor(PdfAnnotation *annotation, float *red, float *green, float *blue) {
        *red = 0;
        *green = 0;
        *blue = 0;
        
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;

        PdfObject* daObj = dict->FindKey("DA");

        if (daObj == nullptr) {
            return;
        }

        if (!daObj->IsString()) {
            return;
        }

        PdfString da = daObj->GetString();
        std::string daValue = da.GetString();
        
        std::vector<std::string>* splitted = PiPiCommon::split(daValue, " ");
        size_t splitSize = splitted->size();
        
        if (splitSize <= 0) {
            return;
        }
        
        std::string colorSign = splitted->back();
        if (colorSign == "g") {
            if (splitSize >= 2) {
                float gray = std::stof((*splitted)[splitSize - 2]);
                
                PiPiColorConverter::ConvertGrayToRGB(gray, red, green, blue);
            }
        } else if (colorSign == "rg") {
            if (splitSize >= 4) {
                *red = std::stof((*splitted)[splitSize - 4]);
                *green = std::stof((*splitted)[splitSize - 3]);
                *blue = std::stof((*splitted)[splitSize - 2]);
            }
        } else if (colorSign == "k") {
            if (splitSize >= 5) {
                float cyan = std::stof((*splitted)[splitSize - 5]);
                float magenta = std::stof((*splitted)[splitSize - 4]);
                float yellow = std::stof((*splitted)[splitSize - 3]);
                float black = std::stof((*splitted)[splitSize - 2]);
                
                PiPiColorConverter::ConvertCMYKToRGB(cyan, magenta, yellow, black, red, green, blue);
            }
        }
        
        delete splitted;
    }

    bool PiPiExtractUtil::ExtractAnnotationBackgroundExists(PdfAnnotation *annotation) {
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;
        
        PdfObject* mkObj = dict->GetKey(PdfName("MK"));
        if (mkObj == nullptr) {
            return false;
        }
        
        if (!mkObj->IsDictionary()) {
            return false;
        }
        
        PdfDictionary& mkRef = mkObj->GetDictionary();
        PdfDictionary* mk = &mkRef;
        
        PdfObject* bcObj = mk->GetKey(PdfName("BG"));
        if (bcObj == nullptr) {
            return false;
        }
        
        if (!bcObj->IsArray()) {
            return false;
        }
        
        return true;
    }

    void PiPiExtractUtil::ExtractAnnotationBackgroundColor(PdfAnnotation *annotation, float *red, float *green, float *blue) {
        *red = 0;
        *green = 0;
        *blue = 0;
        
        PdfDictionary& dictRef = annotation->GetDictionary();
        PdfDictionary* dict = &dictRef;
        
        PdfObject* mkObj = dict->GetKey(PdfName("MK"));
        if (mkObj == nullptr) {
            return;
        }
        
        if (!mkObj->IsDictionary()) {
            return;
        }
        
        PdfDictionary& mkRef = mkObj->GetDictionary();
        PdfDictionary* mk = &mkRef;
        
        PdfObject* bcObj = mk->GetKey(PdfName("BG"));
        if (bcObj == nullptr) {
            return;
        }
        
        if (!bcObj->IsArray()) {
            return;
        }
        
        PdfArray& bcRef = bcObj->GetArray();
        PdfArray* bc = &bcRef;
        
        PiPiColorConverter::ConvertPoDoFoArrayToRGB(bc, red, green, blue);
    }

    PiPiFieldType PiPiExtractUtil::ExtractAnnotationType(PdfAnnotation* annotation) {
        const PdfObject& constObject = annotation->GetObject();
        PdfObject& object = const_cast<PdfObject&>(constObject);

        std::unique_ptr<PdfField> fieldPtr;
        bool created = PdfField::TryCreateFromObject(object, fieldPtr);
        if (!created) {
            return PiPiFieldType::Unknown;
        }

        PdfField* field = fieldPtr.get();

        PdfFieldType type = field->GetType();

        switch (type) {
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

    bool PiPiExtractUtil::ExtractAnnotationTextMultiine(PdfAnnotation *annotation) {
        PiPiFieldType type = ExtractAnnotationType(annotation);
        if (type != PiPiFieldType::TextBox) {
            return false;
        }
        
        const PdfObject& constObject = annotation->GetObject();
        PdfObject& object = const_cast<PdfObject&>(constObject);

        std::unique_ptr<PdfField> fieldPtr;
        bool created = PdfField::TryCreateFromObject(object, fieldPtr);
        if (!created) {
            return false;
        }

        PdfField* field = fieldPtr.get();
        PdfTextBox* textbox = (PdfTextBox*)field;
        
        bool multiline = textbox->IsMultiLine();
        
        return multiline;
    }

    PiPiTextHorizontalAlignment PiPiExtractUtil::ExtractAnnotationTextHorizontalAlignment(PdfAnnotation *annotation) {
        PdfObject& objRef = annotation->GetObject();
        PdfObject* obj = &objRef;
        
        PdfDictionary& dictRef = obj->GetDictionary();
        PdfDictionary* dict = &dictRef;
        
        PdfObject* qObj = dict->FindKey(PdfName("Q"));
        if (qObj == nullptr) {
            return PiPiTextHorizontalAlignment::Left;
        }
        
        int64_t alignmentValue = qObj->GetNumber();
        
        switch (alignmentValue) {
            case 1:
                return PiPiTextHorizontalAlignment::Center;
            case 2:
                return PiPiTextHorizontalAlignment::Right;
            default:
                return PiPiTextHorizontalAlignment::Left;
        }
    }
}
