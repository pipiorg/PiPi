#include "PiPiFieldUtil.h"

namespace PiPi {
    std::map<const std::string, std::vector<PdfField *>*>* PiPiFieldUtil::SearchAllField(PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchAllField(acroform);
    }

    std::map<const std::string, std::vector<PdfField*>*>* PiPiFieldUtil::SearchAllField(PdfAcroForm* acroform) {
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = new std::map<const std::string, std::vector<PdfField*>*>();

        acroform->GetFieldCount();
        for (auto fieldIterator = acroform->begin(); fieldIterator != acroform->end(); fieldIterator.operator++()) {
            PdfField* field = fieldIterator.operator*();
            SearchAllChildrenField(field, fieldMap);
        }

        return fieldMap;
    }

    std::map<const std::string, std::vector<PdfAnnotation*>*>* PiPiFieldUtil::SerachAllFieldAnnotation(PdfMemDocument* document) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SerachAllFieldAnnotation(pages);
    }

    std::map<const std::string, std::vector<PdfAnnotation*>*>* PiPiFieldUtil::SerachAllFieldAnnotation(PdfPageCollection* pages) {
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

    std::vector<PdfField*>* PiPiFieldUtil::SearchField(PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchField(acroform, fieldName);
    }

    std::vector<PdfField*>* PiPiFieldUtil::SearchField(PdfAcroForm* acroform, std::string fieldName) {
        std::vector<PdfField*>* fields = new std::vector<PdfField*>();

        acroform->GetFieldCount();
        for (auto fieldIterator = acroform->begin(); fieldIterator != acroform->end(); fieldIterator.operator++()) {
            PdfField* field = fieldIterator.operator*();
            SearchChildrenField(field, fieldName, fields);
        }

        return fields;
    }

    std::vector<PdfAnnotation*>* PiPiFieldUtil::SearchFieldAnnotation(PdfMemDocument* document, std::string fieldName) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchFieldAnnotation(pages, fieldName);
    }

    std::vector<PdfAnnotation*>* PiPiFieldUtil::SearchFieldAnnotation(PdfPageCollection* pages, std::string fieldName) {
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

    int PiPiFieldUtil::SearchPageIndex(PdfMemDocument* document, PdfPage* page) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchPageIndex(pages, page);
    }

    int PiPiFieldUtil::SearchPageIndex(PdfPageCollection* pages, PdfPage* page) {
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            if (&(pages->GetPageAt(pageIndex)) == page) {
                return pageIndex;
            }
        }
        return -1;
    }

    void PiPiFieldUtil::RemoveAllField(PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();

        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        RemoveAllPageField(pages);
        RemoveAllAcroformField(acroform);
        
        document->CollectGarbage();
    }

    void PiPiFieldUtil::RemoveField(PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();

        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;

        RemovePageField(pages, fieldName);
        RemoveAcroformField(acroform, fieldName);

        document->CollectGarbage();
    }

    void PiPiFieldUtil::CreateField(PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfAcroForm& acroformRef = document->GetOrCreateAcroForm();
        PdfAcroForm* acroform = &acroformRef;

        acroform->SetNeedAppearances(true);

        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;

        PdfPage& pageRef = pages->GetPageAt(pageIndex);

        Rect pageRect = pageRef.GetRect();
        double pageTop = pageRect.GetTop();
        double pageBottom = pageRect.GetBottom();
        double pageHeight = pageTop - pageBottom;

        Rect fieldRect;
        fieldRect.X = x;
        fieldRect.Y = pageHeight - y - height;
        fieldRect.Width = width;
        fieldRect.Height = height;

        std::vector<std::string>* splitted = PiPiCommon::split(fieldName, ".");
        size_t splittedSize = splitted->size();

        if (splittedSize == 1) {
            switch (type) {
            case PiPiFieldType::CheckBox:
                pageRef.CreateField(fieldName, PdfFieldType::CheckBox, fieldRect);
                break;
            case PiPiFieldType::TextBox:
                pageRef.CreateField(fieldName, PdfFieldType::TextBox, fieldRect);
                break;
            }
            
            delete splitted;
            return;
        }
    }

    void PiPiFieldUtil::FlattenAnnotation(PdfAnnotation *annotation) {
        PdfDictionary& dictionary = annotation->GetDictionary();

        std::vector<PdfAppearanceIdentity> apperanceStreams;
        annotation->GetAppearanceStreams(apperanceStreams);

        PdfObject* apperanceStream = nullptr;

        size_t apperanceStreamCount = apperanceStreams.size();
        if (apperanceStreamCount == 1) {
            apperanceStream = const_cast<PdfObject*>(apperanceStreams[0].Object);
        }
        else {
            PdfName as = dictionary.HasKey(PdfName("AS")) ? dictionary.FindKeyAsSafe<PdfName>(PdfName("AS")) : PdfName("Off");
            
            unsigned int normalCount = 0;
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                if (apperanceIdentity.Type == PdfAppearanceType::Normal) {
                    normalCount++;
                }
            }
            
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                
                if (apperanceIdentity.Type == PdfAppearanceType::Normal && apperanceIdentity.State == as) {
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
        painter->SetClipRect(rect);
        painter->DrawXObject(xObjectRef, left, bottom);
        painter->FinishDrawing();

        delete painter;
    }

    void PiPiFieldUtil::SearchAllChildrenField(PdfField* field, std::map<const std::string, std::vector<PdfField*>*>* fieldMap) {
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

    void PiPiFieldUtil::SearchChildrenField(PdfField* field, std::string fieldName, std::vector<PdfField*>* fields) {
        const std::string name = field->GetFullName();
        PdfFieldChildrenCollectionBase& childrens = field->GetChildren();

        unsigned int childrenCount = childrens.GetCount();
        if (childrenCount == 0 && name == fieldName) {
            fields->push_back(field);
            return;
        }

        for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++) {
            PdfField& childrenFieldRef = childrens.GetFieldAt(childrenIndex);
            PdfField* childrenField = &childrenFieldRef;

            SearchChildrenField(childrenField, fieldName, fields);
        }
    }

    void PiPiFieldUtil::RemoveAllPageField(PdfPageCollection* pages) {
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

    void PiPiFieldUtil::RemovePageField(PdfPageCollection* pages, std::string fieldName) {
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

    void PiPiFieldUtil::RemoveAllAcroformField(PdfAcroForm *acroform) {
        if (acroform == nullptr) {
            return;
        }
        
        unsigned int fieldCount = acroform->GetFieldCount();
        for (unsigned int fieldIndex = fieldCount; fieldIndex > 0; fieldIndex--) {
            acroform->RemoveFieldAt(fieldIndex - 1);
        }
    }

    void PiPiFieldUtil::RemoveAcroformField(PdfAcroForm* acroform, std::string fieldName) {
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

    void PiPiFieldUtil::RemoveAcroformChildrenField(PdfField* field, std::string fieldName) {
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