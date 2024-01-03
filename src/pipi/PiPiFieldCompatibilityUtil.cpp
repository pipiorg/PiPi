#include "PiPiFieldCompatibilityUtil.h"

namespace PiPi {
    void PiPiFieldCompatibilityUtil::Patch(PdfMemDocument* document) {
        PatchFieldShouldRestrict(document);
        PatchFieldFieldNameWithDot(document);
    }

    void PiPiFieldCompatibilityUtil::PatchFieldShouldRestrict(PdfMemDocument *document) {
        PdfAcroForm* acroform = &(document->GetOrCreateAcroForm());
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        if (acroformFieldsObj == nullptr) {
            return;
        }
        
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        for (unsigned int idx = 0; idx < acroformFields->size(); idx++) {
            PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(idx));
            InnerPatchFieldShouldRestrict(acroformFieldObj);
        }
    }

    void PiPiFieldCompatibilityUtil::PatchFieldFieldNameWithDot(PdfMemDocument* document) {
    }

    void PiPiFieldCompatibilityUtil::InnerPatchFieldShouldRestrict(PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* fieldKidsObj = fieldDict->FindKey(PdfName("Kids"));
        if (fieldKidsObj == nullptr) {
            return;
        }
        
        PdfArray* fieldKids = &(fieldKidsObj->GetArray());
        
        size_t fieldKidCount = fieldKids->size();
        if (fieldKidCount == 1) {
            PdfObject* fieldKidObj = fieldKids->FindAt(0);
            PdfDictionary* fieldKidDict = &(fieldKidObj->GetDictionary());
            
            if (!fieldKidDict->HasKey(PdfName("T"))) {
                fieldDict->RemoveKey(PdfName("Kids"));
                fieldKidDict->RemoveKey(PdfName("Parent"));
                
                for (auto iterator = fieldKidDict->begin(); iterator != fieldKidDict->end(); iterator.operator++()) {
                    const PdfName key = iterator->first;
                    PdfObject* value = &(iterator->second);
                    
                    if (fieldDict->HasKey(key)) {
                        fieldDict->RemoveKey(key);
                    }
                    
                    fieldDict->AddKey(key, *value);
                }
                
                return;
            }
        }
        
        for (unsigned int idx = 0; idx < fieldKidCount; idx++) {
            PdfObject* fieldKidObj = fieldKids->FindAt(idx);
            InnerPatchFieldShouldRestrict(fieldKidObj);
        }
    }
}
