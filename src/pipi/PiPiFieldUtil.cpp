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
        
        RemoveAllAcroformField(fieldObserver, acroform);
        
        document->CollectGarbage();
    }

    void PiPiFieldUtil::RemoveField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string fieldName) {
        PdfAcroForm* acroform = document->GetAcroForm();

        RemoveAcroformField(fieldObserver, acroform, fieldName);

        document->CollectGarbage();
    }

    void PiPiFieldUtil::CreateField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        std::set<PdfField*>* fields = SearchField(fieldObserver, document, fieldName);
        
        // 目前有一個需要擴展書籤
        if (fields->size() == 1) {
            PiPiFieldUtil::ExpandHierarchicalField(document, fieldName);
        }
        
        bool hierarchical = PiPiCommon::includes(fieldName, ".");
        if (hierarchical) {
            PiPiFieldUtil::CreateHierarchicalField(fieldObserver, annotObserver, document, fieldName, type, pageIndex, x, y, width, height);
        } else {
            PiPiFieldUtil::CreateNonHierarchicalField(fieldObserver, annotObserver, document, fieldName, type, pageIndex, x, y, width, height);
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

    void PiPiFieldUtil::ExpandHierarchicalField(PdfDocument* document, std::string fieldName) {
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

    void PiPiFieldUtil::RestrictHierarchicalField(PdfDocument* document, std::string fieldName) {
        
    }
}
