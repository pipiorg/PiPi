#include "PiPiFieldCompatibilityUtil.h"

namespace PiPi {
    void PiPiFieldCompatibilityUtil::Patch(PdfMemDocument* document) {
        PatchFieldRestrict(document);
        PatchFieldDot(document);
    }

    void PiPiFieldCompatibilityUtil::PatchFieldDot(PdfMemDocument* document) {
        std::vector<PdfObject*>* dotFieldObjs = CollectDotField(document);
        
        for (auto iterator = dotFieldObjs->begin(); iterator != dotFieldObjs->end(); iterator.operator++()) {
            PdfObject* dotFieldObj = *iterator;
            FixDot(document, dotFieldObj);
        }
        
        delete dotFieldObjs;
    }

    void PiPiFieldCompatibilityUtil::PatchFieldRestrict(PdfMemDocument *document) {
        std::vector<PdfObject*>* restrictFieldObjs = CollectRestrictField(document);
        
        for (auto iterator = restrictFieldObjs->begin(); iterator != restrictFieldObjs->end(); iterator.operator++()) {
            PdfObject* restrictFieldObj = *iterator;
            FixRestrict(document, restrictFieldObj);
        }
        
        delete restrictFieldObjs;
    }

    std::vector<PdfObject*>* PiPiFieldCompatibilityUtil::CollectDotField(PdfDocument* document) {
        std::vector<PdfObject*>* dotFieldObjs = new std::vector<PdfObject*>();
        
        PdfAcroForm* acroform = &(document->GetOrCreateAcroForm());
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        if (acroformFieldsObj == nullptr) {
            return dotFieldObjs;
        }
        
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        for (unsigned int idx = 0; idx < acroformFields->size(); idx++) {
            PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(idx));
            CollectDotFieldRecursive(document, acroformFieldObj, dotFieldObjs);
        }
        
        return dotFieldObjs;
    }

    void PiPiFieldCompatibilityUtil::CollectDotFieldRecursive(PdfDocument *document, PdfObject *fieldObj, std::vector<PdfObject *> *dotFieldObjs) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* tObj = fieldDict->FindKey(PdfName("T"));
        if (tObj == nullptr) {
            return;
        }
        
        const PdfString* t = &(tObj->GetString());
        std::string tString = t->GetString();
        if (PiPiStringCommon::includes(tString, ".")) {
            dotFieldObjs->push_back(fieldObj);
        }
        
        PdfObject* kidsObj = fieldDict->FindKey(PdfName("Kids"));
        if (kidsObj == nullptr) {
            return;
        }
        
        PdfArray* kids = &(kidsObj->GetArray());
        for (unsigned int idx = 0; idx < kids->size(); idx++) {
            PdfObject* kidObj = &(kids->MustFindAt(idx));
            CollectDotFieldRecursive(document, kidObj, dotFieldObjs);
        }
    }

    std::vector<PdfObject*>* PiPiFieldCompatibilityUtil::CollectRestrictField(PdfDocument* document) {
        std::vector<PdfObject*>* restrictFieldObjs = new std::vector<PdfObject*>();
        
        PdfAcroForm* acroform = &(document->GetOrCreateAcroForm());
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        if (acroformFieldsObj == nullptr) {
            return restrictFieldObjs;
        }
        
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        for (unsigned int idx = 0; idx < acroformFields->size(); idx++) {
            PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(idx));
            CollectRestrictFieldRecursive(document, acroformFieldObj, restrictFieldObjs);
        }
        
        return restrictFieldObjs;
    }

    void PiPiFieldCompatibilityUtil::CollectRestrictFieldRecursive(PdfDocument* document, PdfObject* fieldObj, std::vector<PdfObject*>* restrictFieldObjs) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* fieldKidsObj = fieldDict->FindKey(PdfName("Kids"));
        if (fieldKidsObj == nullptr) {
            if (!IsRoot(fieldObj)) {
                if (IsNoBrother(fieldObj)) {
                    restrictFieldObjs->push_back(fieldObj);
                }
            }
            
            return;
        }
        
        PdfArray* fieldKids = &(fieldKidsObj->GetArray());
        if (fieldKids->size() == 0) {
            if (!IsRoot(fieldObj)) {
                if (IsNoBrother(fieldObj)) {
                    restrictFieldObjs->push_back(fieldObj);
                }
            }
            
            return;
        }
        
        size_t fieldKidCount = fieldKids->size();
        for (unsigned int idx = 0; idx < fieldKidCount; idx++) {
            PdfObject* fieldKidObj = fieldKids->FindAt(idx);
            CollectRestrictFieldRecursive(document, fieldKidObj, restrictFieldObjs);
        }
    }

    void PiPiFieldCompatibilityUtil::FixDot(PdfDocument *document, PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        if (parentFieldObj == nullptr) {
            FixDotAcroform(document, fieldObj);
        } else {
            FixDotParentField(document, fieldObj);
        }
    }

    void PiPiFieldCompatibilityUtil::FixDotAcroform(PdfDocument *document, PdfObject *fieldObj) {
        PdfAcroForm* acroform = document->GetAcroForm();
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        for (unsigned int idx = 0; idx < acroformFields->size(); idx++) {
            PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(idx));
            if (acroformFieldObj == fieldObj) {
                acroformFields->RemoveAt(idx);
                break;
            }
        }
        
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* tObj = fieldDict->FindKey(PdfName("T"));
        const PdfString* t = &(tObj->GetString());
        std::string tString = t->GetString();
        
        std::vector<std::string>* splits = PiPiStringCommon::split(tString, ".");
        
        std::string lastFieldName = splits->back();
        fieldDict->RemoveKey(PdfName("T"));
        fieldDict->AddKey(PdfName("T"), PdfString(lastFieldName));
        splits->pop_back();
        
        std::string rootFieldName = (*splits)[0];
        PdfObject* rootFieldObj = PrepareFieldAcroform(document, rootFieldName);
        
        PdfObject* parentFieldObj = rootFieldObj;
        for (unsigned int idx = 1; idx < splits->size(); idx++) {
            std::string partialFieldName = (*splits)[idx];
            parentFieldObj = PrepareFieldParentField(document, parentFieldObj, partialFieldName);
        }
        
        delete splits;
        
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());
        
        parentFieldKids->AddIndirect(*fieldObj);
    }

    void PiPiFieldCompatibilityUtil::FixDotParentField(PdfDocument *document, PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        PdfObject* parnetFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        PdfArray* parentFieldKids = &(parnetFieldKidsObj->GetArray());
        
        for (unsigned int idx = 0; idx < parentFieldKids->size(); idx++) {
            PdfObject* parentFieldKidObj = &(parentFieldKids->MustFindAt(idx));
            if (parentFieldKidObj == fieldObj) {
                parentFieldKids->RemoveAt(idx);
                break;
            }
        }
        
        PdfObject* tObj = fieldDict->FindKey(PdfName("T"));
        const PdfString* t = &(tObj->GetString());
        std::string tString = t->GetString();
        
        std::vector<std::string>* splits = PiPiStringCommon::split(tString, ".");
        
        std::string lastFieldName = splits->back();
        fieldDict->RemoveKey(PdfName("T"));
        fieldDict->AddKey(PdfName("T"), PdfString(lastFieldName));
        splits->pop_back();
        
        PdfObject* currentParentFieldObj = parentFieldObj;
        for (unsigned int i = 0; i < splits->size(); i++) {
            std::string partialFieldName = (*splits)[i];
            currentParentFieldObj = PrepareFieldParentField(document, currentParentFieldObj, partialFieldName);
        }
        
        delete splits;
        
        PdfDictionary* currentParentFieldDict = &(currentParentFieldObj->GetDictionary());
        
        PdfObject* currentParentFieldKidsObj = currentParentFieldDict->FindKey(PdfName("Kids"));
        PdfArray* currentParentFieldKids = &(currentParentFieldKidsObj->GetArray());
        
        currentParentFieldKids->AddIndirect(*fieldObj);
    }

    void PiPiFieldCompatibilityUtil::FixExpand(PdfDocument *document, PdfObject *fieldObj) {
        PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());
        
        PdfObject* expandFieldObj = &(indirectObjectList->CreateDictionaryObject());
        PdfDictionary* expandFieldDict = &(expandFieldObj->GetDictionary());
        
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        
        if (parentFieldObj != nullptr) {
            fieldDict->RemoveKey(PdfName("Parent"));
        }
        
        for (unsigned int i = 0; i < PiPiFieldConstants::SpecialHierarchicalFieldKeys.size(); i++) {
            PdfName SpecialHierarchicalFieldKey = PiPiFieldConstants::SpecialHierarchicalFieldKeys[i];
            
            PdfObject* fieldValueObj = fieldDict->FindKey(SpecialHierarchicalFieldKey);
            if (fieldValueObj == nullptr) {
                continue;
            }
            
            expandFieldDict->AddKey(SpecialHierarchicalFieldKey, *fieldValueObj);
            fieldDict->RemoveKey(SpecialHierarchicalFieldKey);
        }
        
        if (parentFieldObj == nullptr) {
            PdfAcroForm* acroform = document->GetAcroForm();
            PdfDictionary* acroformDict = &(acroform->GetDictionary());
            PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
            PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
            
            for (unsigned int idx = 0; idx < acroformFields->size(); idx++) {
                PdfObject* acroformFieldObj = acroformFields->FindAt(idx);
                if (acroformFieldObj == fieldObj) {
                    acroformFields->RemoveAt(idx);
                    break;
                }
            }
            
            acroformFields->AddIndirect(*expandFieldObj);
        } else {
            PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
            PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
            PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());
            
            for (unsigned int idx = 0; idx< parentFieldKids->size(); idx++) {
                PdfObject* parentFieldKidObj = parentFieldKids->FindAt(idx);
                if (parentFieldKidObj == fieldObj) {
                    parentFieldKids->RemoveAt(idx);
                    break;
                }
            }
            
            expandFieldDict->AddKeyIndirect(PdfName("Parent"), *parentFieldObj);
            parentFieldKids->AddIndirect(*expandFieldObj);
        }
        
        expandFieldDict->AddKey(PdfName("Kids"), PdfArray());
        PdfObject* expandFieldKidsObj = expandFieldDict->FindKey(PdfName("Kids"));
        PdfArray* expandFieldKids = &(expandFieldKidsObj->GetArray());
        
        fieldDict->AddKeyIndirect(PdfName("Parent"), *expandFieldObj);
        expandFieldKids->AddIndirect(*fieldObj);
    }

    void PiPiFieldCompatibilityUtil::FixRestrict(PdfDocument *document, PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        fieldDict->RemoveKey(PdfName("Parent"));
        
        for (auto iterator = parentFieldDict->begin(); iterator != parentFieldDict->end(); iterator.operator++()) {
            const PdfName key = iterator->first;
            PdfObject* value = &(iterator->second);
            
            if (key == PdfName("Kids")) {
                continue;
            }
            
            if (fieldDict->HasKey(key)) {
                continue;
            }
            
            fieldDict->AddKey(key, *value);
        }
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());
        
        PdfObject* grandFieldObj = parentFieldDict->FindKey(PdfName("Parent"));
        if (grandFieldObj != nullptr) {
            PdfDictionary* grandFieldDict = &(grandFieldObj->GetDictionary());
            
            PdfObject* grandFieldKidsObj = grandFieldDict->FindKey(PdfName("Kids"));
            PdfArray* grandFieldKids = &(grandFieldKidsObj->GetArray());
            
            for (unsigned int idx = 0; idx < grandFieldKids->size(); idx++) {
                PdfObject* grandFieldKidObj = &(grandFieldKids->MustFindAt(idx));
                if (grandFieldKidObj == parentFieldObj) {
                    grandFieldKids->RemoveAt(idx);
                    break;
                }
            }
            
            grandFieldKids->AddIndirect(*fieldObj);
        } else {
            PdfAcroForm* acroform = document->GetAcroForm();
            PdfDictionary* acroformDict = &(acroform->GetDictionary());
            
            PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
            PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
            
            for (unsigned int idx = 0; idx < acroformFields->size(); idx++) {
                PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(idx));
                if (acroformFieldObj == parentFieldObj) {
                    acroformFields->RemoveAt(idx);
                    break;
                }
            }
            
            acroformFields->AddIndirect(*fieldObj);
        }
    }

    PdfObject* PiPiFieldCompatibilityUtil::PrepareFieldAcroform(PdfDocument *document, std::string name) {
        PdfAcroForm* acroform = document->GetAcroForm();
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        PdfObject* fieldObj = nullptr;
        
        for (unsigned int idx = 0; idx < acroformFields->size(); idx++) {
            PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(idx));
            PdfDictionary* acroformFieldDict = &(acroformFieldObj->GetDictionary());
            
            PdfObject* acroformFieldTObj = acroformFieldDict->FindKey(PdfName("T"));
            const PdfString* acroformFieldT = &(acroformFieldTObj->GetString());
            const std::string acroformFieldTString = acroformFieldT->GetString();
            
            if (acroformFieldTString == name) {
                fieldObj = acroformFieldObj;
                break;
            }
        }
        
        if (fieldObj == nullptr) {
            PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());
            
            fieldObj = &(indirectObjectList->CreateDictionaryObject());
            
            PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
            fieldDict->AddKey(PdfName("T"), PdfString(name));
            fieldDict->AddKey(PdfName("Kids"), PdfArray());
            
            acroformFields->AddIndirect(*fieldObj);
        }
        
        if (IsReal(fieldObj)) {
            throw PiPiFieldCompatibilityException(PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionCode::FixDotFieldFail);
        }
        
        return fieldObj;
    }

    PdfObject* PiPiFieldCompatibilityUtil::PrepareFieldParentField(PdfDocument* document, PdfObject *parentFieldObj, std::string name) {
        if (IsReal(parentFieldObj)) {
            throw PiPiFieldCompatibilityException(PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionCode::FixDotFieldFail);
        }
        
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        if (parentFieldKidsObj == nullptr) {
            parentFieldDict->AddKey(PdfName("Kids"), PdfArray());
            parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        }
        
        PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());
        
        if (parentFieldKids->size() == 1) {
            FixExpand(document, parentFieldObj);
        }
        
        PdfObject* fieldObj = nullptr;
        
        for (unsigned int idx = 0; idx < parentFieldKids->size(); idx++) {
            PdfObject* parentFieldKidObj = &(parentFieldKids->MustFindAt(idx));
            PdfDictionary* parentFieldKidDict = &(parentFieldKidObj->GetDictionary());
            
            PdfObject* parentFieldKidTObj = parentFieldKidDict->FindKey(PdfName("T"));
            const PdfString* parentFieldKidT = &(parentFieldKidTObj->GetString());
            const std::string parentFieldKidTString = parentFieldKidT->GetString();
            
            if (parentFieldKidTString == name) {
                fieldObj = parentFieldKidObj;
                break;
            }
        }
        
        if (fieldObj == nullptr) {
            PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());
            
            fieldObj = &(indirectObjectList->CreateDictionaryObject());
            
            PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
            fieldDict->AddKey(PdfName("T"), PdfString(name));
            fieldDict->AddKey(PdfName("Kids"), PdfArray());
            
            parentFieldKids->AddIndirect(*fieldObj);
        }
        
        if (IsReal(fieldObj)) {
            throw PiPiFieldCompatibilityException(PiPiFieldCompatibilityException::PiPiFieldCompatibilityExceptionCode::FixDotFieldFail);
        }
        
        return fieldObj;
    }

    bool PiPiFieldCompatibilityUtil::IsReal(PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        PdfObject* fieldFTObj = fieldDict->FindKey(PdfName("FT"));
        return fieldFTObj != nullptr;
    }

    bool PiPiFieldCompatibilityUtil::IsRoot(PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        return parentFieldObj == nullptr;
    }

    bool PiPiFieldCompatibilityUtil::IsNoBrother(PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());
        
        return parentFieldKids->size() == 1;
    }
}
