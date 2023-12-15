#include "PiPiFieldUtil.h"

namespace PiPi {
    std::map<const std::string, std::vector<PdfField *>*>* PiPiFieldUtil::SearchAllField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchAllField(fieldObserver, acroform);
    }

    std::map<const std::string, std::vector<PdfField*>*>* PiPiFieldUtil::SearchAllField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform) {
        if (fieldObserver->isObserved()) {
            std::map<const std::string, std::vector<PdfField*>*>* fieldMap;
            bool accessed = fieldObserver->accessAll(&fieldMap);
            if (accessed) {
                return fieldMap;
            }
        }
        
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = new std::map<const std::string, std::vector<PdfField*>*>();

        acroform->GetFieldCount();
        for (auto fieldIterator = acroform->begin(); fieldIterator != acroform->end(); fieldIterator.operator++()) {
            PdfField* field = fieldIterator.operator*();
            SearchAllChildrenField(field, fieldMap);
        }
        
        fieldObserver->observeAll(fieldMap);

        return fieldMap;
    }

    std::map<const std::string, std::vector<PdfAnnotation*>*>* PiPiFieldUtil::SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SerachAllFieldAnnotation(annotObserver, pages);
    }

    std::map<const std::string, std::vector<PdfAnnotation*>*>* PiPiFieldUtil::SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages) {
        
        if (annotObserver->isObserved()) {
            std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap;
            bool accessed = annotObserver->accessAll(&annotMap);
            if (accessed) {
                return annotMap;
            }
        }
        
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
        
        annotObserver->observeAll(annotMap);

        return annotMap;
    }

    std::vector<PdfField*>* PiPiFieldUtil::SearchField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchField(fieldObserver, acroform, fieldName);
    }

    std::vector<PdfField*>* PiPiFieldUtil::SearchField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform, std::string fieldName) {
        if (fieldObserver->isObserved()) {
            std::vector<PdfField*>* fields;
            bool accessed = fieldObserver->access(fieldName, &fields);
            if (accessed) {
                return fields;
            }
        }
        
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = PiPiFieldUtil::SearchAllField(fieldObserver, acroform);
        
        auto findIterator = fieldMap->find(fieldName);
        if (findIterator != fieldMap->end()) {
            return findIterator->second;
        }
        
        return new std::vector<PdfField*>();
    }

    std::vector<PdfAnnotation*>* PiPiFieldUtil::SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchFieldAnnotation(annotObserver, pages, fieldName);
    }

    std::vector<PdfAnnotation*>* PiPiFieldUtil::SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName) {
        if (annotObserver->isObserved()) {
            std::vector<PdfAnnotation*>* resAnnots;
            bool accessed = annotObserver->access(fieldName, &resAnnots);
            if (accessed) {
                return resAnnots;
            }
        }
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* resAnnots = SerachAllFieldAnnotation(annotObserver, pages);
        
        auto findIterator = resAnnots->find(fieldName);
        if (findIterator != resAnnots->end()) {
            return findIterator->second;
        }

        return new std::vector<PdfAnnotation*>();
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

    void PiPiFieldUtil::RemoveAllField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();

        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        RemoveAllPageField(annotObserver, pages);
        RemoveAllAcroformField(fieldObserver, acroform);
        
        document->CollectGarbage();
    }

    void PiPiFieldUtil::RemoveField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();

        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;

        RemovePageField(annotObserver, pages, fieldName);
        RemoveAcroformField(fieldObserver, acroform, fieldName);

        document->CollectGarbage();
    }

    void PiPiFieldUtil::CreateField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        // TODO: solve add feature
        
        /*
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
         */
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

    void PiPiFieldUtil::RemoveAllPageField(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages) {
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;
            
            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;
            
            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = annotCount; annotIndex > 0; annotIndex--) {
                PdfAnnotation& annotRef = annots->GetAnnotAt(annotIndex - 1);
                PdfAnnotation* annot = &annotRef;
                
                std::string fieldName = PiPiExtractUtil::ExtractAnnotationName(annot);
                annotObserver->observe(PiPiAnnotationObserver::PiPiAnnotationObserveType::Remove, fieldName, annot);
                
                annots->RemoveAnnotAt(annotIndex - 1);
            }
        }
    }

    void PiPiFieldUtil::RemovePageField(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName) {
        std::vector<PdfAnnotation*>* tarAnnots = SearchFieldAnnotation(annotObserver, pages, fieldName);

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
                
                PdfAnnotation& annotRef = annots->GetAnnotAt(removeAnnotIndex);
                PdfAnnotation* annot = &annotRef;
                
                std::string fieldName = PiPiExtractUtil::ExtractAnnotationName(annot);
                annotObserver->observe(PiPiAnnotationObserver::PiPiAnnotationObserveType::Remove, fieldName, annot);
                
                annots->RemoveAnnotAt(removeAnnotIndex);
                removeAnnotIndexs->pop_back();
            }

            delete removeAnnotIndexs;
        }

        delete tarAnnots;
    }

    void PiPiFieldUtil::RemoveAllAcroformField(PiPiFieldObserver* fieldObserver, PdfAcroForm *acroform) {
        if (acroform == nullptr) {
            return;
        }
        
        unsigned int fieldCount = acroform->GetFieldCount();
        for (unsigned int fieldIndex = fieldCount; fieldIndex > 0; fieldIndex--) {
            PdfField& fieldRef = acroform->GetFieldAt(fieldIndex);
            PdfField* field = &fieldRef;
            
            std::string fieldName = field->GetFullName();
            
            fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Remove, fieldName, field);
            
            acroform->RemoveFieldAt(fieldIndex - 1);
        }
    }

    void PiPiFieldUtil::RemoveAcroformField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform, std::string fieldName) {
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
                RemoveAcroformChildrenField(fieldObserver, field, fieldName);

                childrenCount = childrens.GetCount();
                if (childrenCount == 0) {
                    removeFieldIndexs->push_back(fieldIndex);
                    continue;
                }
            }
        }

        while (removeFieldIndexs->size()) {
            unsigned int removeFieldIndex = removeFieldIndexs->back();
            
            PdfField& fieldRef = acroform->GetFieldAt(removeFieldIndex);
            PdfField* field = &fieldRef;
            
            std::string fieldName = field->GetFullName();
            
            fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Remove, fieldName, field);
            
            acroform->RemoveFieldAt(removeFieldIndex);
            removeFieldIndexs->pop_back();
        }

        delete removeFieldIndexs;
    }

    void PiPiFieldUtil::RemoveAcroformChildrenField(PiPiFieldObserver* fieldObserver, PdfField* field, std::string fieldName) {
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
                RemoveAcroformChildrenField(fieldObserver, childrenField, fieldName);

                guardsonCount = guardsones->GetCount();
                if (guardsonCount == 0) {
                    removeFieldIndexs->push_back(childrenIndex);
                    continue;
                }
            }
        }

        while (removeFieldIndexs->size()) {
            unsigned int removeFieldIndex = removeFieldIndexs->back();
            
            PdfField& fieldRef = childrens->GetFieldAt(removeFieldIndex);
            PdfField* field = &fieldRef;
            
            std::string fieldName = field->GetFullName();
            
            fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Remove, fieldName, field);
            
            childrens->RemoveFieldAt(removeFieldIndex);
            removeFieldIndexs->pop_back();
        }

        delete removeFieldIndexs;
    }
}
