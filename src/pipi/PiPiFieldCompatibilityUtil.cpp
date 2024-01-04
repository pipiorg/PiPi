#include "PiPiFieldCompatibilityUtil.h"

namespace PiPi {
    void PiPiFieldCompatibilityUtil::Patch(PdfMemDocument* document) {
        PatchFieldDot(document);
        PatchFieldRestrict(document);
    }

    void PiPiFieldCompatibilityUtil::PatchFieldDot(PdfMemDocument* document) {
    }

    void PiPiFieldCompatibilityUtil::PatchFieldRestrict(PdfMemDocument *document) {
        std::vector<PdfObject*>* restrictFieldObjs = CollectRestrictField(document);
        
        for (auto iterator = restrictFieldObjs->begin(); iterator != restrictFieldObjs->end(); iterator.operator++()) {
            PdfObject* restrictFieldObj = *iterator;
            Restrict(document, restrictFieldObj);
        }
        
        delete restrictFieldObjs;
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
            if (IsNoBrother(fieldObj)) {
                restrictFieldObjs->push_back(fieldObj);
            }
            
            return;
        }
        
        PdfArray* fieldKids = &(fieldKidsObj->GetArray());
        if (fieldKids->size() == 0) {
            if (IsNoBrother(fieldObj)) {
                restrictFieldObjs->push_back(fieldObj);
            }
            
            return;
        }
        
        size_t fieldKidCount = fieldKids->size();
        for (unsigned int idx = 0; idx < fieldKidCount; idx++) {
            PdfObject* fieldKidObj = fieldKids->FindAt(idx);
            CollectRestrictFieldRecursive(document, fieldKidObj, restrictFieldObjs);
        }
    }

    void PiPiFieldCompatibilityUtil::Restrict(PdfDocument *document, PdfObject *fieldObj) {
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

    bool PiPiFieldCompatibilityUtil::IsNoBrother(PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());
        
        return parentFieldKids->size() == 1;
    }
}
