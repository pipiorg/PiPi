#include "PiPiFieldUtil.h"

namespace PiPi {
    std::map<const std::string, std::set<PdfField *>*>* PiPiFieldUtil::SearchAllField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchAllField(fieldObserver, acroform);
    }

    std::map<const std::string, std::set<PdfField*>*>* PiPiFieldUtil::SearchAllField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform) {
        if (fieldObserver->isObserved()) {
            std::map<const std::string, std::set<PdfField*>*>* fieldMap;
            bool accessed = fieldObserver->accessAll(&fieldMap);
            if (accessed) {
                return fieldMap;
            }
        }
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = new std::map<const std::string, std::set<PdfField*>*>();

        acroform->GetFieldCount();
        for (auto fieldIterator = acroform->begin(); fieldIterator != acroform->end(); fieldIterator.operator++()) {
            PdfField* field = fieldIterator.operator*();
            SearchAllChildrenField(field, fieldMap);
        }
        
        fieldObserver->observeAll(fieldMap);
        delete fieldMap;
        
        bool accessed = fieldObserver->accessAll(&fieldMap);
        if (accessed) {
            return fieldMap;
        }

        return nullptr;
    }

    std::map<const std::string, std::set<PdfAnnotation*>*>* PiPiFieldUtil::SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SerachAllFieldAnnotation(annotObserver, pages);
    }

    std::map<const std::string, std::set<PdfAnnotation*>*>* PiPiFieldUtil::SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages) {
        
        if (annotObserver->isObserved()) {
            std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap;
            bool accessed = annotObserver->accessAll(&annotMap);
            if (accessed) {
                return annotMap;
            }
        }
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = new std::map<const std::string, std::set<PdfAnnotation*>*>();

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
                    (*annotMap)[name] = new std::set<PdfAnnotation*>();
                }

                std::set<PdfAnnotation*>* resAnnots = (*annotMap)[name];
                resAnnots->insert(annot);

                fieldPtr.reset();
            }
        }
        
        annotObserver->observeAll(annotMap);
        delete annotMap;
        
        bool accessed = annotObserver->accessAll(&annotMap);
        if (accessed) {
            return annotMap;
        }
        
        return nullptr;
    }

    std::set<PdfField*>* PiPiFieldUtil::SearchField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();
        return SearchField(fieldObserver, acroform, fieldName);
    }

    std::set<PdfField*>* PiPiFieldUtil::SearchField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform, std::string fieldName) {
        if (fieldObserver->isObserved()) {
            std::set<PdfField*>* fields;
            bool accessed = fieldObserver->access(fieldName, &fields);
            if (accessed) {
                return fields;
            }
        }
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = PiPiFieldUtil::SearchAllField(fieldObserver, acroform);
        
        auto findIterator = fieldMap->find(fieldName);
        if (findIterator != fieldMap->end()) {
            return findIterator->second;
        }
        
        return new std::set<PdfField*>();
    }

    std::set<PdfAnnotation*>* PiPiFieldUtil::SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchFieldAnnotation(annotObserver, pages, fieldName);
    }

    std::set<PdfAnnotation*>* PiPiFieldUtil::SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName) {
        if (annotObserver->isObserved()) {
            std::set<PdfAnnotation*>* resAnnots;
            bool accessed = annotObserver->access(fieldName, &resAnnots);
            if (accessed) {
                return resAnnots;
            }
        }
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* resAnnots = SerachAllFieldAnnotation(annotObserver, pages);
        
        auto findIterator = resAnnots->find(fieldName);
        if (findIterator != resAnnots->end()) {
            return findIterator->second;
        }

        return new std::set<PdfAnnotation*>();
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
        bool hierarchical = PiPiCommon::includes(fieldName, ".");
        
        if (hierarchical) {
            PiPiFieldUtil::CreateHierarchicalField(fieldObserver, annotObserver, document, fieldName, type, pageIndex, x, y, width, height);
            return;
        }
        
        PiPiFieldUtil::CreateNonHierarchicalField(fieldObserver, annotObserver, document, fieldName, type, pageIndex, x, y, width, height);
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

    void PiPiFieldUtil::SearchAllChildrenField(PdfField* field, std::map<const std::string, std::set<PdfField*>*>* fieldMap) {
        const std::string name = field->GetFullName();
        PdfFieldChildrenCollectionBase& childrensRef = field->GetChildren();
        PdfFieldChildrenCollectionBase* childrens = &childrensRef;

        unsigned int childrenCount = childrens->GetCount();
        if (childrenCount == 0) {
            if ((*fieldMap)[name] == nullptr) {
                (*fieldMap)[name] = new std::set<PdfField*>();
            }

            (*fieldMap)[name]->insert(field);
            return;
        }

        for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++) {
            PdfField& childrenFieldRef = childrens->GetFieldAt(childrenIndex);
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
        std::set<PdfAnnotation*>* tarAnnots = SearchFieldAnnotation(annotObserver, pages, fieldName);

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

            PdfFieldChildrenCollectionBase& guardsonesRef = childrenField->GetChildren();
            PdfFieldChildrenCollectionBase* guardsones = &guardsonesRef;
            unsigned int guardsonCount = guardsones->GetCount();

            if (childrenName == fieldName && guardsonCount == 0) {
                removeFieldIndexs->push_back(childrenIndex);
                continue;
            }

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

    void PiPiFieldUtil::CreateHierarchicalField(PiPiFieldObserver *fieldObserver, PiPiAnnotationObserver *annotObserver, PdfMemDocument *document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        PdfPage& pageRef = pages->GetPageAt(pageIndex);
        PdfPage* page = &pageRef;
        
        PdfAnnotationCollection& annotsRef = page->GetAnnotations();
        PdfAnnotationCollection* annots = &annotsRef;
        
        PdfAcroForm& acroformRef = document->GetOrCreateAcroForm();
        PdfAcroForm* acroform = &acroformRef;
        
        PdfObject& acroformObjRef = acroform->GetObject();
        PdfObject* acroformObj = &acroformObjRef;
        
        PdfDictionary& acroformDictRef = acroformObj->GetDictionary();
        PdfDictionary* acroformDict = &acroformDictRef;
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        
        PdfArray& acroformFieldsRef = acroformFieldsObj->GetArray();
        PdfArray* acroformFields = &acroformFieldsRef;
        
        PdfIndirectObjectList& indirectObjectListRef = document->GetObjects();
        PdfIndirectObjectList* indirectObjectList = &indirectObjectListRef;

        std::vector<std::string>* splits = PiPiCommon::split(fieldName, ".");
        size_t splitCount = splits->size();

        PdfObject* parentFieldObj = nullptr;
        
        // 建立根階層
        std::string rootFieldName = (*splits)[0];
        for (unsigned int i = 0; i < acroformFields->size(); i++) {
            PdfObject& fieldObjRef = acroformFields->MustFindAt(i);
            PdfObject* fieldObj = &fieldObjRef;
            
            PdfDictionary& fieldDictRef = fieldObj->GetDictionary();
            PdfDictionary* fieldDict = &fieldDictRef;
            
            PdfObject* fieldTObj = fieldDict->FindKey(PdfName("T"));
            
            const PdfString& fieldTRef = fieldTObj->GetString();
            const PdfString* fieldT = &fieldTRef;
            
            std::string t = fieldT->GetString();
            if (t == rootFieldName) {
                parentFieldObj = fieldObj;
                break;
            }
        }
        
        if (parentFieldObj == nullptr) {
            PdfObject& fieldObjRef = indirectObjectList->CreateDictionaryObject();
            PdfObject* fieldObj = &fieldObjRef;
            
            PdfDictionary& fieldDictRef = fieldObj->GetDictionary();
            PdfDictionary* fieldDict = &fieldDictRef;
            
            fieldDict->AddKey(PdfName("T"), PdfString(rootFieldName));
            fieldDict->AddKey(PdfName("Kids"), PdfArray());
            
            acroformFields->AddIndirect(fieldObjRef);
            parentFieldObj = fieldObj;
        }
        
        // 建立剩餘階層
        for (size_t i = 1; i < splitCount - 1; i++) {
            std::string partialFieldName = (*splits)[i];
            
            PdfDictionary& parentFieldDictRef = parentFieldObj->GetDictionary();
            PdfDictionary* parentFieldDict = &parentFieldDictRef;
            
            PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
            
            PdfArray& parentFieldKidsRef = parentFieldKidsObj->GetArray();
            PdfArray* parentFieldKids = &parentFieldKidsRef;
            
            PdfObject* newParentFieldObj = nullptr;
            for (unsigned int j = 0; j < parentFieldKids->size(); j++) {
                PdfObject& kidObjRef = parentFieldKids->MustFindAt(j);
                PdfObject* kidObj = &kidObjRef;
                
                PdfDictionary& kidRef = kidObj->GetDictionary();
                PdfDictionary* kid = &kidRef;
                
                PdfObject* kidTObj = kid->FindKey(PdfName("T"));
                
                const PdfString& kidTRef = kidTObj->GetString();
                const PdfString* kidT = &kidTRef;
                
                std::string t = kidT->GetString();
                if (t == partialFieldName) {
                    newParentFieldObj = kidObj;
                }
            }
            
            if (newParentFieldObj == nullptr) {
                PdfObject& kidObjRef = indirectObjectList->CreateDictionaryObject();
                PdfObject* kidObj = &kidObjRef;
                
                PdfDictionary& kidDictRef = kidObj->GetDictionary();
                PdfDictionary* kidDict = &kidDictRef;
                
                kidDict->AddKey(PdfName("T"), PdfString(rootFieldName));
                kidDict->AddKey(PdfName("Kids"), PdfArray());
                kidDict->AddKeyIndirect(PdfName("Parent"), *parentFieldObj);
                
                parentFieldKids->AddIndirect(kidObjRef);
                newParentFieldObj = kidObj;
            }
            
            parentFieldObj = newParentFieldObj;
        }
        
        // 建立欄位外觀
        
        Rect pageRect = page->GetRect();
        double pageHeight = pageRect.Height;
        
        Rect fieldRect = Rect(x, y + pageHeight - height, width, height);
        
        PdfAnnotation& annotRef = annots->CreateAnnot(PdfAnnotationType::Widget, fieldRect);
        PdfAnnotation* annot = &annotRef;
        
        annotObserver->observe(PiPiAnnotationObserver::PiPiAnnotationObserveType::Add, fieldName, annot);
        
        // 建立欄位
        PdfObject& parentFieldObjRef = *parentFieldObj;
        
        PdfDictionary& parentFieldDictRef = parentFieldObj->GetDictionary();
        PdfDictionary* parentFieldDict = &parentFieldDictRef;
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        
        PdfArray& parentFieldKidsRef = parentFieldKidsObj->GetArray();
        PdfArray* parentFieldKids = &parentFieldKidsRef;
        
        std::string lastFieldName = (*splits)[splits->size() - 1];
        
        PdfObject& lastFieldObjRef = annot->GetObject();
        PdfObject* lastFieldObj = &lastFieldObjRef;
        
        PdfDictionary& lastFieldDictRef = lastFieldObj->GetDictionary();
        PdfDictionary* lastFieldDict = &lastFieldDictRef;
        
        lastFieldDict->AddKey(PdfName("T"), PdfString(lastFieldName));
        lastFieldDict->AddKeyIndirect(PdfName("Parent"), parentFieldObjRef);
        
        switch (type) {
            case PiPiFieldType::TextBox:
                lastFieldDict->AddKey(PdfName("FT"), PdfName("Tx"));
                break;
            case PiPiFieldType::CheckBox:
                lastFieldDict->AddKey(PdfName("FT"), PdfName("Btn"));
                break;
        }
        
        parentFieldKids->AddIndirect(lastFieldObjRef);
        
        std::unique_ptr<PdfField> lastFieldPtr;
        bool created = PdfField::TryCreateFromObject(lastFieldObjRef, lastFieldPtr);
        if (!created) {
            delete splits;
            return;
        }
        
        PdfField* lastField = lastFieldPtr.release();
        
        fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Add, fieldName, lastField);
        
        delete splits;
    }

    void PiPiFieldUtil::CreateNonHierarchicalField(PiPiFieldObserver *fieldObserver, PiPiAnnotationObserver *annotObserver, PdfMemDocument *document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        PdfPage& pageRef = pages->GetPageAt(pageIndex);
        PdfPage* page = &pageRef;
        
        Rect pageRect = page->GetRect();
        double pageHeight = pageRect.Height;
        
        Rect fieldRect = Rect(x, y + pageHeight - height, width, height);
        
        PdfField* field = nullptr;
        switch (type) {
            case PiPiFieldType::TextBox:
                field = &(page->CreateField(fieldName, PdfFieldType::TextBox, fieldRect));
                break;
            case PiPiFieldType::CheckBox:
                field = &(page->CreateField(fieldName, PdfFieldType::CheckBox, fieldRect));
                break;
            case PiPiFieldType::Unknown:
                return;
        }
        
        fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Add, fieldName, field);
        
        PdfObject& fieldObjRef = field->GetObject();
        PdfObject* fieldObj = &fieldObjRef;
        
        std::unique_ptr<PdfAnnotation> annotPtr;
        bool created = PdfAnnotation::TryCreateFromObject(fieldObjRef, annotPtr);
        if (!created) {
            return;
        }
        
        PdfAnnotation* annot = annotPtr.release();
        
        annotObserver->observe(PiPiAnnotationObserver::PiPiAnnotationObserveType::Add, fieldName, annot);
    }
}
