#include "PiPiFieldUtil.h"

namespace PiPi {
    const std::vector<PdfName> PiPiFieldUtil::SpecialHierarchicalFieldKeys {
        PdfName("T"),
        PdfName("FT"),
        PdfName("Ff"),
        PdfName("Opt"),
        PdfName("V")
    };

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

    void PiPiFieldUtil::RemoveAllField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document) {
        PdfAcroForm* acroform = document->GetAcroForm();
        
        if (acroform == nullptr) {
            return;
        }
        
        PdfDictionary& acroformDictRef = acroform->GetDictionary();
        PdfDictionary* acroformDict = &acroformDictRef;
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        
        PdfArray& acroformFieldsRef = acroformFieldsObj->GetArray();
        PdfArray* acroformFields = &acroformFieldsRef;
        
        unsigned int acroformFieldsCount = (unsigned int) acroformFields->size();
        for (unsigned int i = acroformFieldsCount - 1; i >= 0 ; i--) {
            acroformFields->RemoveAt(i);
        }
        
        fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Clear, nullptr, nullptr);
        
        document->CollectGarbage();
    }

    void PiPiFieldUtil::RemoveField(PiPiFieldObserver *fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument *document, std::string fieldName) {
        RemoveField(fieldObserver, annotObserver, document, fieldName, -1);
    }

    void PiPiFieldUtil::RemoveField(PiPiFieldObserver *fieldObserver, PiPiAnnotationObserver *annotObserver, PdfMemDocument *document, std::string fieldName, long pageIndex) {
        RemoveField(fieldObserver, annotObserver, document, fieldName, pageIndex, -1, -1);
    }

    void PiPiFieldUtil::RemoveField(PiPiFieldObserver *fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument *document, std::string fieldName, long pageIndex, double x, double y) {
        RemoveField(fieldObserver, annotObserver, document, fieldName, pageIndex, x, y, -1, -1);
    }

    void PiPiFieldUtil::RemoveField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, long pageIndex, double x, double y, double width, double height) {
        PdfAcroForm* acroform = document->GetAcroForm();

        if (acroform == nullptr) {
            return;
        }
        
        std::set<PdfField*>* fields = SearchField(fieldObserver, document, fieldName);
        std::set<PdfAnnotation*>* annots = PiPiAnnotationUtil::SearchFieldAnnotation(annotObserver, document, fieldName);
        
        std::map<PdfField*, PdfAnnotation*>* map = new std::map<PdfField*, PdfAnnotation*>();
        for (auto fieldIterator = fields->begin(); fieldIterator != fields->end(); fieldIterator.operator++()) {
            for (auto annotIterator = annots->begin(); annotIterator != annots->end(); annotIterator.operator++()) {
                PdfField* field = *fieldIterator;
                PdfAnnotation* annot = *annotIterator;
                if (&(field->GetObject()) == &(annot->GetObject())) {
                    map->insert(std::pair<PdfField*, PdfAnnotation*>(field, annot));
                }
            }
        }
        
        for (auto iterator = map->begin(); iterator != map->end(); iterator.operator++()) {
            PdfField* field = iterator->first;
            PdfAnnotation* annot = iterator->second;
            
            unsigned int aPageIndex = PiPiPageUtil::SearchPageIndex(document, annot->GetPage());
            double ax = PiPiExtractUtil::ExtractAnnotationX(annot);
            double ay = PiPiExtractUtil::ExtractAnnotationY(annot);
            double aWidth = PiPiExtractUtil::ExtractAnnotationWidth(annot);
            double aHeight = PiPiExtractUtil::ExtractAnnotationHeight(annot);
            
            bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                (x == ax || x == -1) &&
                (y == ay || y == -1) &&
                (width == aWidth || width == -1) &&
                (height == aHeight || height == -1);
            
            if (!matched) {
                continue;
            }
            
            RemoveChildrenField(fieldObserver, document, field);
        }
        
        delete map;
        
        document->CollectGarbage();
    }

    void PiPiFieldUtil::CreateField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        std::set<PdfField*>* fields = SearchField(fieldObserver, document, fieldName);
        
        // 目前有一個需要擴展書籤
        if (fields->size() == 1) {
            ExpandChildrenField(document, fieldName);
        }
        
        bool hierarchical = PiPiCommon::includes(fieldName, ".");
        if (hierarchical) {
            CreateChildrenField(fieldObserver, annotObserver, document, fieldName, type, pageIndex, x, y, width, height);
        } else {
            CreateNonChildrenField(fieldObserver, annotObserver, document, fieldName, type, pageIndex, x, y, width, height);
        }
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

    void PiPiFieldUtil::RemoveChildrenField(PiPiFieldObserver *fieldObserver, PdfDocument *document, PdfField *field) {
        std::string fieldName = field->GetFullName();
        
        PdfObject& fieldObjRef = field->GetObject();
        PdfObject* fieldObj = &fieldObjRef;
        PdfDictionary& fieldDictRef = fieldObj->GetDictionary();
        PdfDictionary* fieldDict = &fieldDictRef;
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        if (parentFieldObj != nullptr) {
            PdfObject& parentFieldObjRef = *parentFieldObj;
            PdfDictionary& parentFieldDictRef = parentFieldObj->GetDictionary();
            PdfDictionary* parentFieldDict = &parentFieldDictRef;
            
            std::unique_ptr<PdfField> parentFieldPtr;
            PdfField::TryCreateFromObject(parentFieldObjRef, parentFieldPtr);
            PdfField* parentField = parentFieldPtr.get();
            
            PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
            PdfArray& parentFieldKidsRef = parentFieldKidsObj->GetArray();
            PdfArray* parentFieldKids = &parentFieldKidsRef;
            size_t parentFieldKidCount = parentFieldKids->size();
            
            unsigned int pos = 0;
            for (unsigned int idx = 0; idx < parentFieldKidCount; idx++) {
                if (fieldObj == &(parentFieldKids->MustFindAt(idx))) {
                    pos = idx;
                    break;
                }
            }
            
            parentFieldKids->RemoveAt(pos);
            
            fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Remove, fieldName, field);
            
            if (parentFieldKidCount - 1 == 1) {
                RestrictChildrenField(document, fieldName);
            }
            
            if (parentFieldKidCount - 1 == 0) {
                RemoveChildrenField(fieldObserver, document, parentField);
            }
        } else {
            PdfAcroForm* acroform = document->GetAcroForm();
            PdfDictionary& acroformDictRef = acroform->GetDictionary();
            PdfDictionary* acroformDict = &acroformDictRef;
            
            PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
            PdfArray& acroformFieldsRef = acroformFieldsObj->GetArray();
            PdfArray* acroformFields = &acroformFieldsRef;
            unsigned int acroformFieldCount = (unsigned int)acroformFields->size();
            
            unsigned int pos = 0;
            for (unsigned int idx = 0; idx < acroformFieldCount; idx++) {
                if (fieldObj == &(acroformFields->MustFindAt(idx))) {
                    pos = idx;
                    break;
                }
                
                acroformFields->RemoveAt(idx);
                fieldObserver->observe(PiPiFieldObserver::PiPiFieldObserveType::Remove, fieldName, field);
            }
        }
    }

    void PiPiFieldUtil::CreateChildrenField(PiPiFieldObserver *fieldObserver, PiPiAnnotationObserver *annotObserver, PdfMemDocument *document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        std::set<PdfField*>* fields = SearchField(fieldObserver, document, fieldName);
        size_t cFieldSize = fields->size();
        bool sameName = cFieldSize >= 1;
        
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
        for (size_t i = 1; i < splitCount - (sameName ? 0 : 1); i++) {
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
                    break;
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
        
        PdfObject& lastFieldObjRef = annot->GetObject();
        PdfObject* lastFieldObj = &lastFieldObjRef;
        
        PdfDictionary& lastFieldDictRef = lastFieldObj->GetDictionary();
        PdfDictionary* lastFieldDict = &lastFieldDictRef;
        
        lastFieldDict->AddKeyIndirect(PdfName("Parent"), parentFieldObjRef);
        
        if (sameName) {
            for (unsigned int i = 0; i < SpecialHierarchicalFieldKeys.size(); i++) {
                PdfName SpecialHierarchicalFieldKey = SpecialHierarchicalFieldKeys[i];
                if (lastFieldDict->HasKey(SpecialHierarchicalFieldKey)) {
                    lastFieldDict->RemoveKey(SpecialHierarchicalFieldKey);
                }
            }
        }
        
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

    void PiPiFieldUtil::CreateNonChildrenField(PiPiFieldObserver *fieldObserver, PiPiAnnotationObserver *annotObserver, PdfMemDocument *document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
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

    void PiPiFieldUtil::ExpandChildrenField(PdfDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();
        
        PdfDictionary& acroformDictRef = acroform->GetDictionary();
        PdfDictionary* acroformDict = &acroformDictRef;
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        
        PdfArray& acroformFieldsRef = acroformFieldsObj->GetArray();
        PdfArray* acroformFields = &acroformFieldsRef;
        
        PdfIndirectObjectList& indirectObjectListRef = document->GetObjects();
        PdfIndirectObjectList* indirectObjectList = &indirectObjectListRef;
        
        std::vector<std::string>* splits = PiPiCommon::split(fieldName, ".");
        
        PdfObject* parentFieldObj = nullptr;
        PdfObject* fieldObj = nullptr;
        unsigned int childIndex = 0;
        
        for (unsigned int i = 0; i < splits->size(); i++) {
            std::string partialFieldName = (*splits)[i];
            
            if (i == 0) {
                for (unsigned int j = 0; j < acroformFields->size(); j++) {
                    PdfObject& acroformFieldObjRef = acroformFields->MustFindAt(j);
                    PdfObject* acroformFieldObj = &acroformFieldObjRef;
                    
                    PdfDictionary& acroformFieldDictRef = acroformFieldObj->GetDictionary();
                    PdfDictionary* acroformFieldDict = &acroformFieldDictRef;
                    
                    PdfObject* acroformFieldTObj = acroformFieldDict->FindKey(PdfName("T"));
                    
                    const PdfString& acroformFieldTRef = acroformFieldTObj->GetString();
                    const PdfString* acroformFieldT = &acroformFieldTRef;
                    
                    std::string t = acroformFieldT->GetString();
                    
                    if (t == partialFieldName) {
                        parentFieldObj = nullptr;
                        fieldObj = acroformFieldObj;
                        childIndex = j;
                        break;
                    }
                }
                continue;
            }
            
            PdfDictionary& fieldDictObj = fieldObj->GetDictionary();
            PdfDictionary* fieldDict = &fieldDictObj;
            
            PdfObject* fieldKidsObj = fieldDict->FindKey(PdfName("Kids"));
            
            PdfArray& fieldKidsRef = fieldKidsObj->GetArray();
            PdfArray* fieldKids = &fieldKidsRef;
            
            for (unsigned int j = 0; j < fieldKids->size(); j++) {
                PdfObject& fieldKidObjRef = fieldKids->MustFindAt(j);
                PdfObject* fieldKidObj = &fieldKidObjRef;
                
                PdfDictionary& fieldKidDictRef = fieldKidObj->GetDictionary();
                PdfDictionary* fieldKidDict = &fieldKidDictRef;
                
                PdfObject* fieldKidTObj = fieldKidDict->FindKey(PdfName("T"));
                
                const PdfString& fieldKidTRef = fieldKidTObj->GetString();
                const PdfString* fieldKidT = &fieldKidTRef;
                
                std::string t = fieldKidT->GetString();
                if (t == partialFieldName) {
                    parentFieldObj = fieldObj;
                    fieldObj = fieldKidObj;
                    childIndex = j;
                    break;
                }
            }
        }
        
        PdfDictionary& fieldDictRef = fieldObj->GetDictionary();
        PdfDictionary* fieldDict = &fieldDictRef;
        
        PdfDictionary& parentFieldDictObj = parentFieldObj->GetDictionary();
        PdfDictionary* parentFieldDict = &parentFieldDictObj;
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        
        PdfArray& parentFieldKidsRef = parentFieldKidsObj->GetArray();
        PdfArray* parentFieldKids = &parentFieldKidsRef;
        
        // 從爸爸把自己移掉
        if (parentFieldObj == nullptr) {
            acroformFields->RemoveAt(childIndex);
        } else {
            parentFieldKids->RemoveAt(childIndex);
            fieldDict->RemoveKey(PdfName("Parent"));
        }
        
        // 宣告擴展層
        PdfObject& expandFieldObjRef = indirectObjectList->CreateDictionaryObject();
        PdfObject* expandFieldObj = &expandFieldObjRef;
        
        PdfDictionary& expandFieldDictRef = expandFieldObj->GetDictionary();
        PdfDictionary* expandFieldDict = &expandFieldDictRef;
        
        // 移動部分屬性至擴展層
        for (unsigned int i = 0; i < SpecialHierarchicalFieldKeys.size(); i++) {
            PdfName SpecialHierarchicalFieldKey = SpecialHierarchicalFieldKeys[i];
            
            PdfObject* fieldValueObj = fieldDict->FindKey(SpecialHierarchicalFieldKey);
            if (fieldValueObj == nullptr) {
                continue;
            }
            
            expandFieldDict->AddKey(SpecialHierarchicalFieldKey, *fieldValueObj);
            fieldDict->RemoveKey(SpecialHierarchicalFieldKey);
        }
        
        // 從爸爸加上擴展層
        if (parentFieldObj == nullptr) {
            acroformFields->AddIndirect(expandFieldObjRef);
        } else {
            parentFieldKids->AddIndirect(expandFieldObjRef);
            expandFieldDict->AddKeyIndirect(PdfName("Parent"), *parentFieldObj);
        }
        
        // 從擴展層加上自己
        expandFieldDict->AddKey(PdfName("Kids"), PdfArray());
        PdfObject* expandFieldKidsObj = expandFieldDict->FindKey(PdfName("Kids"));
        PdfArray& expandFieldKidsRef = expandFieldKidsObj->GetArray();
        PdfArray* expandFieldKids = &expandFieldKidsRef;
        
        fieldDict->AddKeyIndirect(PdfName("Parent"), expandFieldObjRef);
        expandFieldKids->AddIndirect(*fieldObj);
    }

    void PiPiFieldUtil::RestrictChildrenField(PdfDocument* document, std::string fieldName) {
        PdfAcroForm& acroformRef = document->GetOrCreateAcroForm();
        PdfAcroForm* acroform = &acroformRef;
        
        PdfObject& acroformObjRef = acroform->GetObject();
        PdfObject* acroformObj = &acroformObjRef;
        
        PdfDictionary& acroformDictRef = acroformObj->GetDictionary();
        PdfDictionary* acroformDict = &acroformDictRef;
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        
        PdfArray& acroformFieldsRef = acroformFieldsObj->GetArray();
        PdfArray* acroformFields = &acroformFieldsRef;
        
        std::vector<std::string>* splits = PiPiCommon::split(fieldName, ".");
        size_t splitCount = splits->size();

        PdfObject* parentFieldObj = nullptr;
        unsigned int parentFieldIndex = 0;
        
        PdfObject* fieldObj = nullptr;
        
        // 找到根階層
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
                parentFieldIndex = i;
                parentFieldObj = fieldObj;
                
                PdfObject* fieldKidsObj = fieldDict->FindKey(PdfName("Kids"));
                fieldObj = fieldKidsObj != nullptr
                    ? &(fieldKidsObj->GetArray().MustFindAt(0))
                    : nullptr;
                
                break;
            }
        }
        
        // 建立剩餘階層
        for (size_t i = 1; i < splitCount; i++) {
            std::string partialFieldName = (*splits)[i];
            
            PdfDictionary& parentFieldDictRef = parentFieldObj->GetDictionary();
            PdfDictionary* parentFieldDict = &parentFieldDictRef;
            
            PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
            
            PdfArray& parentFieldKidsRef = parentFieldKidsObj->GetArray();
            PdfArray* parentFieldKids = &parentFieldKidsRef;
            
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
                    parentFieldIndex = j;
                    parentFieldObj = kidObj;
                    
                    PdfObject* grandsObj = kid->FindKey(PdfName("Kids"));
                    fieldObj = grandsObj != nullptr
                        ? &(grandsObj->GetArray().MustFindAt(0))
                        : nullptr;
                    
                    break;
                }
            }
        }
        
        PdfObject& parentFieldObjRef = *parentFieldObj;
        PdfDictionary& parentFieldDictRef = parentFieldObj->GetDictionary();
        PdfDictionary* parentFieldDict = &parentFieldDictRef;
        
        PdfObject& fieldObjRef = *fieldObj;
        PdfDictionary& fieldDictRef = fieldObj->GetDictionary();
        PdfDictionary* fieldDict = &fieldDictRef;
        
        fieldDict->RemoveKey(PdfName("Parent"));
        parentFieldDict->RemoveKey(PdfName("Kids"));
        
        for (auto iterator = parentFieldDict->begin(); iterator != parentFieldDict->end(); iterator.operator++()) {
            const PdfName name = iterator->first;
            const PdfObject& value = iterator->second;
            
            if (fieldDict->HasKey(name)) {
                continue;
            }
            
            fieldDict->AddKey(name, value);
        }
        
        PdfObject* grandFieldObj = parentFieldDict->FindKey(PdfName("Parent"));
        if (grandFieldObj == nullptr) {
            acroformFields->RemoveAt(parentFieldIndex);
            
            acroformFields->AddIndirect(*fieldObj);
            
        } else {
            PdfDictionary& grandFieldDictRef = grandFieldObj->GetDictionary();
            PdfDictionary* grandFieldDict = &grandFieldDictRef;
            
            PdfObject* grandFieldKidsObj = grandFieldDict->FindKey(PdfName("Kids"));
            PdfObject& grandFieldKidsObjRef = *grandFieldKidsObj;
            
            PdfArray& grandFieldKidsRef = grandFieldKidsObj->GetArray();
            PdfArray* grandFieldKids = &grandFieldKidsRef;
            
            grandFieldKids->RemoveAt(parentFieldIndex);
            
            grandFieldKids->AddIndirect(*fieldObj);
        }
    }
}
