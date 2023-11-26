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
}