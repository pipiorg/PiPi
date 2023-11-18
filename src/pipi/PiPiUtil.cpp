#include "PiPiUtil.h"

namespace PiPi {
    std::map<const std::string, std::vector<PdfField *>*>* PiPiUtil::SearchAllField(PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchAllField(acroform);
    }

    std::map<const std::string, std::vector<PdfField*>*>* PiPiUtil::SearchAllField(PdfAcroForm* acroform) {
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = new std::map<const std::string, std::vector<PdfField*>*>();

        acroform->GetFieldCount();
        for (auto fieldIterator = acroform->begin(); fieldIterator != acroform->end(); fieldIterator.operator++()) {
            PdfField* field = fieldIterator.operator*();
            SearchAllChildrenField(field, fieldMap);
        }

        return fieldMap;
    }

    std::map<const std::string, std::vector<PdfAnnotation*>*>* PiPiUtil::SerachAllFieldAnnotation(PdfMemDocument* document) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SerachAllFieldAnnotation(pages);
    }

    std::map<const std::string, std::vector<PdfAnnotation*>*>* PiPiUtil::SerachAllFieldAnnotation(PdfPageCollection* pages) {
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = new std::map<const std::string, std::vector<PdfAnnotation*>*>();

        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;

            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;

            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++) {
                PdfAnnotation& annotRef = annots->GetAnnotAt(annotIndex);
                PdfAnnotation* annot = &annotRef;

                const PdfObject& constObject = annot->GetObject();
                PdfObject& object = const_cast<PdfObject&>(constObject);

                std::unique_ptr<PdfField> fieldPtr;
                bool created = PdfField::TryCreateFromObject(object, fieldPtr);
                if (!created) {
                    continue;
                }

                PdfField* field = fieldPtr.get();

                std::string name = field->GetFullName();

                if ((*annotMap)[name] == nullptr) {
                    (*annotMap)[name] = new std::vector<PdfAnnotation*>();
                }

                std::vector<PdfAnnotation*>* resAnnots = (*annotMap)[name];
                resAnnots->push_back(annot);

                fieldPtr.reset();
            }
        }

        return annotMap;
    }

    PdfField* PiPiUtil::SearchField(PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchField(acroform, fieldName);
    }

    PdfField* PiPiUtil::SearchField(PdfAcroForm* acroform, std::string fieldName) {
        std::vector<const PdfField*>* fields = new std::vector<const PdfField*>();

        acroform->GetFieldCount();
        for (auto fieldIterator = acroform->begin(); fieldIterator != acroform->end(); fieldIterator.operator++()) {
            PdfField* field = fieldIterator.operator*();
            return SearchChildrenField(field, fieldName);
        }

        return nullptr;
    }

    std::vector<PdfAnnotation*>* PiPiUtil::SearchFieldAnnotation(PdfMemDocument* document, std::string fieldName) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchFieldAnnotation(pages, fieldName);
    }

    std::vector<PdfAnnotation*>* PiPiUtil::SearchFieldAnnotation(PdfPageCollection* pages, std::string fieldName) {
        std::vector<PdfAnnotation*>* resAnnots = new std::vector<PdfAnnotation*>();

        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;

            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;

            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++) {
                PdfAnnotation& annotRef = annots->GetAnnotAt(annotIndex);
                PdfAnnotation* annot = &annotRef;

                const PdfObject& constObject = annot->GetObject();
                PdfObject& object = const_cast<PdfObject&>(constObject);

                std::unique_ptr<PdfField> fieldPtr;
                bool created = PdfField::TryCreateFromObject(object, fieldPtr);
                if (!created) {
                    continue;
                }

                PdfField* field = fieldPtr.get();

                std::string name = field->GetFullName();
                if (name == fieldName) {
                    resAnnots->push_back(annot);
                }

                fieldPtr.reset();
            }
        }

        return resAnnots;
    }

    int PiPiUtil::SearchPageIndex(PdfMemDocument* document, PdfPage* page) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchPageIndex(pages, page);
    }

    int PiPiUtil::SearchPageIndex(PdfPageCollection* pages, PdfPage* page) {
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            if (&(pages->GetPageAt(pageIndex)) == page) {
                return pageIndex;
            }
        }
        return -1;
    }

    double PiPiUtil::ExtractPageWidth(PdfPage* page) {
        Rect rect = page->GetRect();

        double left = rect.GetLeft();
        double right = rect.GetRight();

        double width = right - left;

        return width;
    }

    double PiPiUtil::ExtractPageHeight(PdfPage* page) {
        Rect rect = page->GetRect();

        double top = rect.GetTop();
        double bottom = rect.GetBottom();

        double height = top - bottom;

        return height;
    }

    double PiPiUtil::ExtractPageX(PdfPage* page) {
        Rect rect = page->GetRect();

        double x = rect.GetLeft();

        return x;
    }

    double PiPiUtil::ExtractPageY(PdfPage* page) {
        Rect rect = page->GetRect();

        double y = rect.GetTop();

        return y;
    }

    std::string PiPiUtil::ExtractAnnotationFontName(PdfAnnotation* annotation) {
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

    float PiPiUtil::ExtractAnnotationFontSize(PdfAnnotation* annotation) {
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

    double PiPiUtil::ExtractAnnotationWidth(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double left = rect.GetLeft();
        double right = rect.GetRight();

        double width = right - left;

        return width;
    }

    double PiPiUtil::ExtractAnnotationHeight(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double top = rect.GetTop();
        double bottom = rect.GetBottom();

        double height = top - bottom;

        return height;
    }

    double PiPiUtil::ExtractAnnotationX(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double y = rect.GetTop();

        return y;
    }
    double PiPiUtil::ExtractAnnotationY(PdfAnnotation* annotation) {
        Rect rect = annotation->GetRect();

        double x = rect.GetLeft();

        return x;
    }

    PiPiFieldType PiPiUtil::ExtractAnnotationType(PdfAnnotation* annotation) {
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

    void PiPiUtil::RemoveAllField(PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();

        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        RemoveAllPageField(pages);
        RemoveAllAcroformField(acroform);
        
        document->CollectGarbage();
    }

    void PiPiUtil::RemoveField(PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();

        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;

        RemovePageField(pages, fieldName);
        RemoveAcroformField(acroform, fieldName);

        document->CollectGarbage();
    }

    void PiPiUtil::FlattenAnnotation(PdfAnnotation *annotation) {
        PdfDictionary& dictionary = annotation->GetDictionary();

        std::vector<PdfAppearanceIdentity> apperanceStreams;
        annotation->GetAppearanceStreams(apperanceStreams);

        PdfObject* apperanceStream = nullptr;

        size_t apperanceStreamCount = apperanceStreams.size();
        if (apperanceStreamCount == 1) {
            apperanceStream = const_cast<PdfObject*>(apperanceStreams[0].Object);
        }
        else {
            PdfName as = dictionary.HasKey(PdfName("AS")) ? dictionary.FindKeyAsSafe<PdfName>(PdfName("AS")) : PdfName("No");
            PdfName v = dictionary.HasKey(PdfName("V")) ? dictionary.FindKeyAsSafe<PdfName>(PdfName("V")) : PdfName("No");
            
            unsigned int normalCount = 0;
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                if (apperanceIdentity.Type == PdfAppearanceType::Normal) {
                    normalCount++;
                }
            }
            
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                
                bool found = (apperanceIdentity.Type == PdfAppearanceType::Normal && normalCount == 1 && as == v) || (apperanceIdentity.Type == PdfAppearanceType::Normal && apperanceIdentity.State == as);
                
                if (found) {
                    apperanceStream = const_cast<PdfObject*>(apperanceIdentity.Object);
                }
            }
        }

        if (apperanceStream == nullptr) {
            return;
        }

        PdfObject& apperanceStreanRef = *apperanceStream;

        std::unique_ptr<PdfXObjectForm> xObjectUniquePtr;
        bool xObjectCreated = PdfXObjectForm::TryCreateFromObject(apperanceStreanRef, xObjectUniquePtr);
        if (!xObjectCreated) {
            return;
        }

        PdfXObjectForm* xObject = xObjectUniquePtr.get();
        PdfXObjectForm& xObjectRef = *xObject;

        Rect rect = annotation->GetRect();
        double left = rect.GetLeft();
        double bottom = rect.GetBottom();

        PdfPage* page = annotation->GetPage();
        PdfPage& pageRef = *page;

        PdfPainter* painter = new PdfPainter();
        painter->SetCanvas(pageRef);
        painter->DrawXObject(xObjectRef, left, bottom);
        painter->FinishDrawing();

        delete painter;
    }

    void PiPiUtil::SearchAllChildrenField(PdfField* field, std::map<const std::string, std::vector<PdfField*>*>* fieldMap) {
        const std::string name = field->GetFullName();
        PdfFieldChildrenCollectionBase& childrens = field->GetChildren();

        unsigned int childrenCount = childrens.GetCount();
        if (childrenCount == 0) {
            if ((*fieldMap)[name] == nullptr) {
                (*fieldMap)[name] = new std::vector<PdfField*>();
            }

            (*fieldMap)[name]->push_back(field);
            return;
        }

        for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++) {
            PdfField& childrenFieldRef = childrens.GetFieldAt(childrenIndex);
            PdfField* childrenField = &childrenFieldRef;

            SearchAllChildrenField(childrenField, fieldMap);
        }
    }

    PdfField* PiPiUtil::SearchChildrenField(PdfField* field, std::string fieldName) {
        const std::string name = field->GetFullName();
        PdfFieldChildrenCollectionBase& childrens = field->GetChildren();

        unsigned int childrenCount = childrens.GetCount();
        if (childrenCount == 0 && name == fieldName) {
            return field;
        }

        for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++) {
            PdfField& childrenFieldRef = childrens.GetFieldAt(childrenIndex);
            PdfField* childrenField = &childrenFieldRef;

            PdfField* sChildrenField = SearchChildrenField(childrenField, fieldName);
            if (sChildrenField != nullptr) {
                return sChildrenField;
            }
        }

        return nullptr;
    }

    void PiPiUtil::RemoveAllPageField(PdfPageCollection* pages) {
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;
            
            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;
            
            unsigned int annotCount = annots->GetCount();
            for (unsigned annotIndex = annotCount; annotIndex > 0; annotIndex--) {
                annots->RemoveAnnotAt(annotIndex - 1);
            }
        }
    }

    void PiPiUtil::RemovePageField(PdfPageCollection* pages, std::string fieldName) {
        std::vector<PdfAnnotation*>* tarAnnots = SearchFieldAnnotation(pages, fieldName);

        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;

            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;

            std::vector<unsigned int>* removeAnnotIndexs = new std::vector<unsigned int>();

            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++) {
                PdfAnnotation& annotRef = annots->GetAnnotAt(annotIndex);
                PdfAnnotation* annot = &annotRef;

                bool found = std::find(tarAnnots->begin(), tarAnnots->end(), annot) != tarAnnots->end();
                if (found) {
                    removeAnnotIndexs->push_back(annotIndex);
                }
            }

            while (removeAnnotIndexs->size()) {
                unsigned int removeAnnotIndex = removeAnnotIndexs->back();
                annots->RemoveAnnotAt(removeAnnotIndex);
                removeAnnotIndexs->pop_back();
            }

            delete removeAnnotIndexs;
        }

        delete tarAnnots;
    }

    void PiPiUtil::RemoveAllAcroformField(PdfAcroForm *acroform) {
        if (acroform == nullptr) {
            return;
        }
        
        unsigned int fieldCount = acroform->GetFieldCount();
        for (unsigned int fieldIndex = fieldCount; fieldIndex > 0; fieldIndex--) {
            acroform->RemoveFieldAt(fieldIndex - 1);
        }
    }

    void PiPiUtil::RemoveAcroformField(PdfAcroForm* acroform, std::string fieldName) {
        if (acroform == nullptr) {
            return;
        }
        
        std::vector<unsigned int>* removeFieldIndexs = new std::vector<unsigned int>();

        unsigned int fieldCount = acroform->GetFieldCount();
        for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; fieldIndex++) {
            PdfField& fieldRef = acroform->GetFieldAt(fieldIndex);
            PdfField* field = &fieldRef;

            std::string name = field->GetFullName();
            if (name == fieldName) {
                removeFieldIndexs->push_back(fieldIndex);
                continue;
            }

            PdfFieldChildrenCollectionBase& childrens = field->GetChildren();
            unsigned int childrenCount = childrens.GetCount();

            if (childrenCount != 0) {
                RemoveAcroformChildrenField(field, fieldName);

                childrenCount = childrens.GetCount();
                if (childrenCount == 0) {
                    removeFieldIndexs->push_back(fieldIndex);
                    continue;
                }
            }
        }

        while (removeFieldIndexs->size()) {
            unsigned int removeFieldIndex = removeFieldIndexs->back();
            acroform->RemoveFieldAt(removeFieldIndex);
            removeFieldIndexs->pop_back();
        }

        delete removeFieldIndexs;
    }

    void PiPiUtil::RemoveAcroformChildrenField(PdfField* field, std::string fieldName) {
        PdfFieldChildrenCollectionBase& childrensRef = field->GetChildren();
        PdfFieldChildrenCollectionBase* childrens = &childrensRef;

        unsigned int childrenCount = 0;

        std::vector<unsigned int>* removeFieldIndexs = new std::vector<unsigned int>();

        childrenCount = childrens->GetCount();
        for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++) {
            PdfField& childrenFieldRef = childrens->GetFieldAt(childrenIndex);
            PdfField* childrenField = &childrenFieldRef;

            std::string childrenName = childrenField->GetFullName();
            if (childrenName == fieldName) {
                removeFieldIndexs->push_back(childrenIndex);
                continue;
            }

            PdfFieldChildrenCollectionBase& guardsonesRef = childrenField->GetChildren();
            PdfFieldChildrenCollectionBase* guardsones = &guardsonesRef;
            unsigned int guardsonCount = guardsones->GetCount();

            if (guardsonCount != 0) {
                RemoveAcroformChildrenField(childrenField, fieldName);

                guardsonCount = guardsones->GetCount();
                if (guardsonCount == 0) {
                    removeFieldIndexs->push_back(childrenIndex);
                    continue;
                }
            }
        }

        while (removeFieldIndexs->size()) {
            unsigned int removeFieldIndex = removeFieldIndexs->back();
            childrens->RemoveFieldAt(removeFieldIndex);
            removeFieldIndexs->pop_back();
        }

        delete removeFieldIndexs;
    }
}
