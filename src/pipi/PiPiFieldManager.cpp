#include "PiPiFieldManager.h"

namespace PiPi {
    const std::vector<PdfName> PiPiFieldManager::SpecialHierarchicalFieldKeys {
        PdfName("T"),
        PdfName("FT"),
        PdfName("Ff"),
        PdfName("Opt"),
        PdfName("V")
    };

    PiPiFieldManager::PiPiFieldManager(PdfMemDocument* document) {
        this->InitDocument(document);
        this->InitFieldMap();
        this->InitAnnotationMap();
        this->InitFieldBridgeMap();
        this->InitAnnotationBridgeMap();
    }

    PiPiFieldManager::~PiPiFieldManager() {
        if (this->fieldMap != nullptr) {
            for (auto mapIterator = this->fieldMap->begin(); mapIterator != this->fieldMap->end(); mapIterator.operator++()) {
                delete mapIterator->second;
                mapIterator->second = nullptr;
            }
            
            delete this->fieldMap;
            this->fieldMap = nullptr;
        }
        
        
        if (this->annotMap != nullptr) {
            for (auto mapIterator = this->annotMap->begin(); mapIterator != this->annotMap->end(); mapIterator.operator++()) {
                delete mapIterator->second;
                mapIterator->second = nullptr;
            }
            
            delete this->annotMap;
            this->annotMap = nullptr;
        }
        
        if (this->fieldBridgeMap != nullptr) {
            delete this->fieldBridgeMap;
            this->fieldBridgeMap = nullptr;
        }
        
        if (this->annotBridgeMap != nullptr) {
            delete this->annotBridgeMap;
            this->annotBridgeMap = nullptr;
        }
    }

    bool PiPiFieldManager::IsOperable() {
        return this->document != nullptr;
    }
    
    std::map<const std::string, std::set<PdfField*>*>* PiPiFieldManager::SearchAllField() {
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        std::map<const std::string, std::set<PdfField*>*>* outFieldMap = new std::map<const std::string, std::set<PdfField*>*>();
        for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            PiPiManagedFields* managedFields = mapIterator->second;
            
            if (!managedFields->IsReal()) {
                continue;
            }
            
            std::set<PdfField*>* fields = managedFields->AccessFields();
            std::set<PdfField*>* outFields = new std::set<PdfField*>();
            
            outFields->insert(fields->begin(), fields->end());
            delete fields;
            
            outFieldMap->insert(std::pair<const std::string, std::set<PdfField*>*>(fieldName, outFields));
        }
        
        return outFieldMap;
    }

    std::set<PdfField*>* PiPiFieldManager::SearchField(std::string fieldName) {
        auto findIterator = this->fieldMap->find(fieldName);
        if (findIterator == this->fieldMap->end()) {
            return new std::set<PdfField*>();
        }
        
        PiPiManagedFields* managedFields = findIterator->second;
        
        std::set<PdfField*>* fields = managedFields->AccessFields();
        std::set<PdfField*>* outFields = new std::set<PdfField*>();
        
        outFields->insert(fields->begin(), fields->end());
        
        delete fields;
        
        return outFields;
    }

    void PiPiFieldManager::RemoveAllField() {
        PdfMemDocument* document = this->document;
        
        PdfAcroForm* acroform = &(document->GetOrCreateAcroForm());
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        long acroformFieldsCount = acroformFields->size();
        for (long i = acroformFieldsCount - 1; i >= 0 ; i--) {
            acroformFields->RemoveAt((unsigned int)i);
        }
        
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        for (auto iterator = fieldMap->begin(); iterator != fieldMap->end(); iterator.operator++()) {
            delete iterator->second;
            iterator->second = nullptr;
        }
        
        fieldMap->clear();
        
        std::map<PdfObject*, PdfField*>* fieldBridgeMap = this->fieldBridgeMap;
        fieldBridgeMap->clear();
        
        this->RemoveAllFieldAnnotation();
    }


    void PiPiFieldManager::RemoveField(std::string fieldName) {
        this->RemoveField(fieldName, -1);
    }

    void PiPiFieldManager::RemoveField(std::string fieldName, long pageIndex) {
        this->RemoveField(fieldName, pageIndex, -1, -1);
    }

    void PiPiFieldManager::RemoveField(std::string fieldName, long pageIndex, double x, double y) {
        this->RemoveField(fieldName, pageIndex, x, y, -1, -1);
    }

    void PiPiFieldManager::RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        
        auto findIterator = fieldMap->find(fieldName);
        if (findIterator == fieldMap->end()) {
            return;
        }
        
        PiPiManagedFields* managedFields = findIterator->second;
        std::set<PdfField*>* fields = managedFields->AccessFields();
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            PdfAnnotation* annot = this->BridgeFieldToAnnotation(field);
            
            PdfPage* page = &(annot->MustGetPage());
            
            int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
            double ax = PiPiExtractUtil::ExtractAnnotationX(annot);
            double ay = PiPiExtractUtil::ExtractAnnotationY(annot);
            double aWidth = PiPiExtractUtil::ExtractAnnotationWidth(annot);
            double aHeight = PiPiExtractUtil::ExtractAnnotationHeight(annot);
            
            bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                (x == ax || x == -1) &&
                (y == ay || y == -1) &&
                (width == aWidth || width == -1) &&
                (height == aHeight || height == -1);
            
            if (matched) {
                PdfObject* fieldObj = &(field->GetObject());
                this->InnerRemoveField(fieldObj);
            }
        }
        
        delete fields;
        
        this->RemoveFieldAnnotation(fieldName, pageIndex, x, y, width, height);
    }

    PdfField* PiPiFieldManager::CreateField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        if (this->IsDuplicateFieldExists(fieldName)) {
            throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::DuplicateFieldExists);
        }
        
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        auto findIterator = fieldMap->find(fieldName);
        if (findIterator != fieldMap->end()) {
            PiPiManagedFields* managedFields = findIterator->second;
            
            if (managedFields->IsReal()) {
                std::set<PdfField*>* fields = managedFields->AccessFields();
                
                size_t fieldCount = fields->size();
                if (fieldCount == 1) {
                    PdfField* field = *(fields->begin());
                    PdfObject* fieldObj = &(field->GetObject());
                    this->ExpandField(fieldObj);
                }
                
                delete fields;
            }
        }
        
        PdfObject* fieldObj = this->InnerCreateRealField(fieldName, type, pageIndex, x, y, width, height);
        PdfField* field = this->BridgeObjectToField(fieldObj);
        
        return field;
    }

    void PiPiFieldManager::RenameField(std::string oldFieldName, std::string newFieldName) {
        if (this->IsDuplicateFieldExists(newFieldName)) {
            throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::DuplicateFieldExists);
        }
        
        // TODO: 非常非常的麻煩的操作
        
        
        this->RenameFieldMap(oldFieldName, newFieldName);
        
        this->RenameFieldAnnotation(oldFieldName, newFieldName);
    }

    std::map<const std::string, std::set<PdfAnnotation*>*>* PiPiFieldManager::SearchAllFieldAnnotation() {
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        std::map<const std::string, std::set<PdfAnnotation*>*>* outAnnotMap = new std::map<const std::string, std::set<PdfAnnotation*>*>();
        
        for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            PiPiManagedAnnotations* managedAnnots = mapIterator->second;
            
            std::set<PdfAnnotation*>* annots = managedAnnots->AccessAnnotations();
            std::set<PdfAnnotation*>* outAnnots = new std::set<PdfAnnotation*>();
            
            outAnnots->insert(annots->begin(), annots->end());
            
            delete annots;
            
            outAnnotMap->insert(std::pair<const std::string, std::set<PdfAnnotation*>*>(fieldName, outAnnots));
        }
        
        return outAnnotMap;
    }

    std::set<PdfAnnotation*>* PiPiFieldManager::SearchFieldAnnotation(std::string fieldName) {
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        
        auto findIterator = annotMap->find(fieldName);
        if (findIterator == annotMap->end()) {
            return new std::set<PdfAnnotation*>();
        }
        
        PiPiManagedAnnotations* managedAnnots = findIterator->second;
        std::set<PdfAnnotation*>* annots = managedAnnots->AccessAnnotations();
        std::set<PdfAnnotation*>* outAnnots = new std::set<PdfAnnotation*>();
        
        outAnnots->insert(annots->begin(), annots->end());
        
        delete annots;
        
        return outAnnots;
    }

    void PiPiFieldManager::RemoveAllFieldAnnotation() {
        PdfDocument* document = this->document;
        PdfPageCollection* pages = &(document->GetPages());
        
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage* page = &(pages->GetPageAt(pageIndex));
            PdfAnnotationCollection* annots = &(page->GetAnnotations());
            
            long annotCount = annots->GetCount();
            for (long annotIndex = annotCount - 1; annotIndex >= 0; annotIndex--) {
                annots->RemoveAnnotAt((unsigned int)annotIndex);
            }
        }
        
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        for (auto iterator = annotMap->begin(); iterator != annotMap->end(); iterator.operator++()) {
            delete iterator->second;
            iterator->second = nullptr;
        }
        
        annotMap->clear();
        
        std::map<PdfObject*, PdfAnnotation*>* annotBridgeMap = this->annotBridgeMap;
        annotBridgeMap->clear();
    }

    void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName) {
        this->RemoveFieldAnnotation(fieldName, -1);
    }

    void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName, long pageIndex) {
        this->RemoveFieldAnnotation(fieldName, pageIndex, -1, -1);
    }

    void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y) {
        this->RemoveFieldAnnotation(fieldName, pageIndex, x, y, -1, -1);
    }

    void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        
        auto findIterator = annotMap->find(fieldName);
        if (findIterator == annotMap->end()) {
            return;
        }
        
        PiPiManagedAnnotations* managedAnnots = findIterator->second;
        std::set<PdfAnnotation*>* annots = managedAnnots->AccessAnnotations();
        for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++()) {
            PdfAnnotation* annot = *iterator;
            PdfObject* annotObj = &(annot->GetObject());
            
            PdfPage* page = &(annot->MustGetPage());
            
            int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
            double ax = PiPiExtractUtil::ExtractAnnotationX(annot);
            double ay = PiPiExtractUtil::ExtractAnnotationY(annot);
            double aWidth = PiPiExtractUtil::ExtractAnnotationWidth(annot);
            double aHeight = PiPiExtractUtil::ExtractAnnotationHeight(annot);
            
            bool matched = (pageIndex == aPageIndex || pageIndex == -1) &&
                (x == ax || x == -1) &&
                (y == ay || y == -1) &&
                (width == aWidth || width == -1) &&
                (height == aHeight || height == -1);
            
            
            if (matched) {
                std::vector<unsigned int>* removePageAnnotIndexs = new std::vector<unsigned int>();
                
                PdfAnnotationCollection* pageAnnots = &(page->GetAnnotations());
                size_t pageAnnotCount = pageAnnots->GetCount();
                for (long pageAnnotIndex = pageAnnotCount - 1; pageAnnotIndex >= 0; pageAnnotIndex--) {
                    PdfAnnotation* pageAnnot = &(pageAnnots->GetAnnotAt((unsigned int)pageAnnotIndex));
                    if (&(pageAnnot->GetObject()) == annotObj) {
                        removePageAnnotIndexs->push_back((unsigned int)pageAnnotIndex);
                    }
                }
                
                size_t removePageAnnotIndexCount = removePageAnnotIndexs->size();
                for (unsigned int removePageAnnotIndexIndex = 0; removePageAnnotIndexIndex < removePageAnnotIndexCount; removePageAnnotIndexIndex++) {
                    pageAnnots->RemoveAnnotAt((*removePageAnnotIndexs)[removePageAnnotIndexIndex]);
                    this->RemoveAnnotationMap(annotObj, fieldName);
                }
                
                delete removePageAnnotIndexs;
            }
        }
        
        delete annots;
    }

    PdfAnnotation* PiPiFieldManager::CreateFieldAnnotation(std::string fieldName, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
        
        PdfPageCollection* pages = &(document->GetPages());
        size_t pageCount = pages->GetCount();
        if (pageIndex >= pageCount || pageIndex < 0) {
            throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPageIndex);
        }
        
        PdfPage* page = &(pages->GetPageAt(pageIndex));
        
        double pageHeight = PiPiExtractUtil::ExtractPageHeight(page);
        
        PdfAnnotationCollection* annots = &(page->GetAnnotations());
        
        PdfAnnotation* annot = &(annots->CreateAnnot(PdfAnnotationType::Widget, Rect(x, pageHeight - y - height, width, height)));
        PdfObject* object = &(annot->GetObject());
        
        this->AddAnnotationMap(object, annot, fieldName);
        
        return annot;
    }

    void PiPiFieldManager::RenameFieldAnnotation(std::string oldFieldName, std::string newFieldName) {
        this->RenameAnnotationMap(oldFieldName, newFieldName);
    }

    PdfField* PiPiFieldManager::BridgeAnnotationToField(PdfAnnotation *annot) {
        std::map<PdfObject*, PdfField*>* fieldBridgeMap = this->fieldBridgeMap;
        PdfObject* object = &(annot->GetObject());
        
        return this->BridgeObjectToField(object);
    }

    PdfField* PiPiFieldManager::BridgeObjectToField(PdfObject *object) {
        auto findIterator = fieldBridgeMap->find(object);
        if (findIterator == fieldBridgeMap->end()) {
            return nullptr;
        }
        
        return findIterator->second;
    }

    PdfAnnotation* PiPiFieldManager::BridgeFieldToAnnotation(PdfField *field) {
        std::map<PdfObject*, PdfAnnotation*>* annotBridgeMap = this->annotBridgeMap;
        PdfObject* object = &(field->GetObject());
        return this->BridgeObjectToAnnotation(object);
    }

    PdfAnnotation* PiPiFieldManager::BridgeObjectToAnnotation(PdfObject *object) {
        auto findIterator = annotBridgeMap->find(object);
        if (findIterator == annotBridgeMap->end()) {
            return nullptr;
        }
        
        return findIterator->second;
    }

    void PiPiFieldManager::InitDocument(PdfMemDocument* document) {
        PiPiFieldCompatibilityUtil::Patch(document);
        
        this->document = document;
    }

    void PiPiFieldManager::InitFieldMap() {
        this->fieldMap = new std::map<const std::string, PiPiManagedFields*>();
        
        PdfMemDocument* document = this->document;
        
        PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());
        
        PdfAcroForm* acroform = &(document->GetOrCreateAcroForm());
        PdfObject* acroformObj = &(acroform->GetObject());
        PdfDictionary* acroformDict = &(acroformObj->GetDictionary());
        
        PdfObject* fieldsObj = acroformDict->FindKey(PdfName("Fields"));
        if (fieldsObj == nullptr) {
            fieldsObj = &(indirectObjectList->CreateArrayObject());
            acroformDict->AddKeyIndirect(PdfName("Fields"), *fieldsObj);
        }
        
        PdfArray* fields = &(fieldsObj->GetArray());
        
        for (unsigned int idx = 0; idx < fields->size(); idx++) {
            PdfObject* fieldObj = &(fields->MustFindAt(idx));
            this->InnerSearchAllField(fieldObj);
        }
    }

    void PiPiFieldManager::InitAnnotationMap() {
        PdfDocument* document = this->document;
        
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = new std::map<const std::string, PiPiManagedAnnotations*>();
        
        PdfPageCollection* pages = &(document->GetPages());
        
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage* page = &(pages->GetPageAt(pageIndex));
            
            PdfAnnotationCollection* annots = &(page->GetAnnotations());

            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++) {
                PdfAnnotation* annot = &(annots->GetAnnotAt(annotIndex));

                const PdfObject& constObject = annot->GetObject();
                PdfObject& object = const_cast<PdfObject&>(constObject);

                std::unique_ptr<PdfField> fieldPtr;
                bool created = PdfField::TryCreateFromObject(object, fieldPtr);
                if (!created) {
                    continue;
                }

                PdfField* field = fieldPtr.get();

                std::string name = field->GetFullName();
    
                auto findIterator = annotMap->find(name);
                if (findIterator == annotMap->end()) {
                    PiPiManagedAnnotations* managedAnnots = new PiPiManagedAnnotations(annot);
                    annotMap->insert(std::pair<const std::string, PiPiManagedAnnotations*>(name, managedAnnots));
                    continue;
                }
                
                PiPiManagedAnnotations* managedAnnots = findIterator->second;
                managedAnnots->ManageAnnotation(annot);
            }
        }
        
        this->annotMap = annotMap;
    }

    void PiPiFieldManager::InitFieldBridgeMap() {
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        
        std::map<PdfObject*, PdfField*>* fieldBridgeMap = new std::map<PdfObject*, PdfField*>();
        
        for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++()) {
            PiPiManagedFields* managedFields = mapIterator->second;
            
            std::set<PdfField*>* fields = managedFields->AccessFields();
            
            for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
                PdfField* field = *iterator;
                PdfObject* object = &(field->GetObject());
                
                fieldBridgeMap->insert(std::pair<PdfObject*, PdfField*>(object, field));
            }
            
            delete fields;
        }
        
        this->fieldBridgeMap = fieldBridgeMap;
    }

    void PiPiFieldManager::InitAnnotationBridgeMap() {
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        
        std::map<PdfObject*, PdfAnnotation*>* annotBridgeMap = new std::map<PdfObject*, PdfAnnotation*>();
        
        for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
            PiPiManagedAnnotations* managedAnnots = mapIterator->second;
            
            std::set<PdfAnnotation*>* annots = managedAnnots->AccessAnnotations();
            
            for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++()) {
                PdfAnnotation* annot = *iterator;
                PdfObject* object = &(annot->GetObject());
                
                annotBridgeMap->insert(std::pair<PdfObject*, PdfAnnotation*>(object, annot));
            }
            
            delete annots;
        }
        
        this->annotBridgeMap = annotBridgeMap;
    }

    void PiPiFieldManager::InnerSearchAllField(PdfObject *fieldObj) {
        std::unique_ptr<PdfField> fieldPtr;
        bool created = PdfField::TryCreateFromObject(*fieldObj, fieldPtr);
        if (!created) {
            throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPdfFieldObject);
        }
        
        PdfField* field = fieldPtr.release();
        
        const std::string name = field->GetFullName();
        PdfFieldChildrenCollectionBase* childrens = &(field->GetChildren());

        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        
        unsigned int childrenCount = childrens->GetCount();
        if (childrenCount == 0) {
            auto findIterator = fieldMap->find(name);
            if (findIterator == fieldMap->end()) {
                PiPiManagedFields* manageFields = new PiPiManagedFields(field, true);
                fieldMap->insert(std::pair<const std::string, PiPiManagedFields*>(name, manageFields));
                return;
            }
            
            PiPiManagedFields* manageFields = findIterator->second;
            manageFields->ManageField(field);
            return;
        }
        
        bool allKidEnd = true;
        for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++) {
            PdfField* childrenField = &(childrens->GetFieldAt(childrenIndex));
            PdfObject* childrenObject = &(childrenField->GetObject());
            PdfDictionary* childrenDict = &(childrenObject->GetDictionary());
            
            if (childrenDict->HasKey(PdfName("T"))) {
                allKidEnd = false;
                break;
            }
        }
        
        if (!allKidEnd) {
            PiPiManagedFields* manageFields = new PiPiManagedFields(field, false);
            fieldMap->insert(std::pair<const std::string, PiPiManagedFields*>(name, manageFields));
        }

        for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++) {
            PdfField* childrenField = &(childrens->GetFieldAt(childrenIndex));
            PdfObject* childrenObject = &(childrenField->GetObject());
            
            InnerSearchAllField(childrenObject);
        }
    }

    void PiPiFieldManager::InnerRemoveField(PdfObject *fieldObj) {
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        if (parentFieldObj == nullptr) {
            this->InnerRemoveFieldFromAcroform(fieldObj);
        } else {
            this->InnerRemoveFieldFromParentField(fieldObj);
        }
    }

    void PiPiFieldManager::InnerRemoveFieldFromAcroform(PdfObject *fieldObj) {
        PdfField* field = this->BridgeObjectToField(fieldObj);
        
        PdfAcroForm* acroform = &(document->GetOrCreateAcroForm());
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        size_t acroformFieldCount = acroformFields->size();
        unsigned int pos = 0;
        for (unsigned int idx = 0; idx < acroformFieldCount; idx++) {
            if (fieldObj == &(acroformFields->MustFindAt(idx))) {
                acroformFields->RemoveAt(idx);
                break;
            }
        }
        
        std::string fieldName = field->GetFullName();
        this->RemoveFieldMap(fieldObj, fieldName);
    }

    void PiPiFieldManager::InnerRemoveFieldFromParentField(PdfObject *fieldObj) {
        PdfField* field = this->BridgeObjectToField(fieldObj);
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
        PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());
        
        size_t parentFieldKidCount = parentFieldKids->size();
        for (unsigned int idx = 0; idx < parentFieldKidCount; idx++) {
            if (fieldObj == &(parentFieldKids->MustFindAt(idx))) {
                parentFieldKids->RemoveAt(idx);
                break;
            }
        }
        
        std::string fieldName = field->GetFullName();
        this->RemoveFieldMap(fieldObj, fieldName);
        
        if (parentFieldKidCount - 1 == 1) {
            PdfObject* brotherFieldObj = &(parentFieldKids->MustFindAt(0));
            PdfDictionary* brotherFieldDict = &(brotherFieldObj->GetDictionary());
            
            if (!brotherFieldDict->HasKey(PdfName("T"))) {
                this->RestrictField(brotherFieldObj);
            }
        }
        
        if (parentFieldKidCount - 1 == 0) {
            this->InnerRemoveField(parentFieldObj);
        }
    }

    PdfObject* PiPiFieldManager::InnerCreateFakeField(std::string fieldName) {
        PdfMemDocument* document = this->document;
        
        PdfField* cField = nullptr;
        
        std::vector<std::string>* splits = PiPiStringCommon::split(fieldName, ".");
        std::vector<std::string>* createSplits = new std::vector<std::string>();
        
        while (cField == nullptr && splits->size()) {
            std::string partialFieldName = PiPiStringCommon::join(splits, ".");
            
            auto findIterator = fieldMap->find(partialFieldName);
            if (findIterator != fieldMap->end()) {
                PiPiManagedFields* managedFields = findIterator->second;
                
                std::set<PdfField*>* fields = managedFields->AccessFields();
                
                cField = *(fields->begin());
                
                delete fields;
                
                break;
            }
            
            std::string lastPartialFieldName = splits->back();
            
            createSplits->push_back(lastPartialFieldName);
            splits->pop_back();
        }
        
        delete splits;
        
        PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());
        
        if (cField == nullptr) {
            std::string firstPartialFieldName = createSplits->back();
            
            PdfAcroForm* acroform = &(document->GetOrCreateAcroForm());
            PdfDictionary* acroformDict = &(acroform->GetDictionary());
            
            PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
            PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
            
            
            PdfObject* fieldObj = &(indirectObjectList->CreateDictionaryObject());
            PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
            
            fieldDict->AddKey(PdfName("Kids"), PdfArray());
            fieldDict->AddKey(PdfName("T"), PdfName(firstPartialFieldName));
            acroformFields->AddIndirect(*fieldObj);
            
            std::unique_ptr<PdfField> fieldPtr;
            PdfField::TryCreateFromObject(*fieldObj, fieldPtr);
            PdfField* field = fieldPtr.release();
            
            std::string fieldName = field->GetFullName();
            
            this->AddFieldMap(fieldObj, field, fieldName, false);
            
            cField = field;
            
            createSplits->pop_back();
        }
        
        std::reverse(createSplits->begin(), createSplits->end());
        
        for (auto iterator = createSplits->begin(); iterator != createSplits->end(); iterator.operator++()) {
            std::string partialFieldName = *iterator;
            
            PdfObject* cFieldObj = &(cField->GetObject());
            PdfDictionary* cFieldDict = &(cField->GetDictionary());
            
            PdfObject* cFieldKidsObj = cFieldDict->FindKey(PdfName("Kids"));
            PdfArray* cFieldKids = &(cFieldKidsObj->GetArray());
            
            PdfObject* fieldObj = &(indirectObjectList->CreateDictionaryObject());
            PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
            
            fieldDict->AddKey(PdfName("Kids"), PdfArray());
            fieldDict->AddKey(PdfName("T"), PdfName(partialFieldName));
            fieldDict->AddKeyIndirect(PdfName("Parent"), *cFieldObj);
            
            cFieldKids->AddIndirect(*fieldObj);
            
            std::unique_ptr<PdfField> fieldPtr;
            PdfField::TryCreateFromObject(*fieldObj, fieldPtr);
            PdfField* field = fieldPtr.release();
            
            std::string fieldName = field->GetFullName();
            
            this->AddFieldMap(fieldObj, field, fieldName, false);
            
            cField = field;
        }
        
        delete createSplits;
        
        PdfObject* cFieldObj = &(cField->GetObject());
        
        return cFieldObj;
    }

    PdfObject* PiPiFieldManager::InnerCreateRealField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        
        PdfAnnotation* annot = this->CreateFieldAnnotation(fieldName, pageIndex, x, y, width, height);
        PdfObject* object = &(annot->GetObject());
        PdfDictionary* dict = &(object->GetDictionary());
        
        switch (type) {
            case PiPiFieldType::TextBox:
                dict->AddKey(PdfName("FT"), PdfName("Tx"));
                break;
            case PiPiFieldType::CheckBox:
                dict->AddKey(PdfName("FT"), PdfName("Btn"));
                break;
            case PiPiFieldType::Unknown:
            default:
                throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPdfFieldType);
        }
        
        // 已有欄位 -> 同名書籤 -> 從兄弟找爸爸
        // 未有書籤 -> 全新書籤 -> 建立假爸爸
        auto findIterator = fieldMap->find(fieldName);
        if (findIterator != fieldMap->end()) {
            PiPiManagedFields* managedFields = findIterator->second;
            
            std::set<PdfField*>* brotherFields = managedFields->AccessFields();
            
            PdfField* brotherField = *(brotherFields->begin());
            PdfObject* brotherFieldObj = &(brotherField->GetObject());
            PdfDictionary* brotherFieldDict = &(brotherFieldObj->GetDictionary());
            
            delete brotherFields;
            
            PdfObject* parentObject = brotherFieldDict->FindKey(PdfName("Parent"));
            PdfDictionary* parentDict = &(parentObject->GetDictionary());
            
            PdfObject* parentKidsObject = parentDict->FindKey(PdfName("Kids"));
            PdfArray* parentKids = &(parentKidsObject->GetArray());
            
            dict->AddKeyIndirect(PdfName("Parent"), *parentObject);
            parentKids->AddIndirect(*object);
        } else {
            std::vector<std::string>* splits = PiPiStringCommon::split(fieldName, ".");
            
            std::string lastFieldName = splits->back();
            
            splits->pop_back();
            std::string parentFieldName = PiPiStringCommon::join(splits, ".");
            
            delete splits;
            
            PdfObject* parentObject = this->InnerCreateFakeField(parentFieldName);
            PdfDictionary* parentDict = &(parentObject->GetDictionary());
            
            PdfObject* parentKidsObject = parentDict->FindKey(PdfName("Kids"));
            PdfArray* parentKids = &(parentKidsObject->GetArray());
            
            dict->AddKeyIndirect(PdfName("Parent"), *parentObject);
            dict->AddKey(PdfName("T"), PdfString(lastFieldName));
            
            parentKids->AddIndirect(*object);
        }
        
        std::unique_ptr<PdfField> fieldPtr;
        PdfField::TryCreateFromObject(*object, fieldPtr);
        PdfField* field = fieldPtr.release();
        
        this->AddFieldMap(object, field, fieldName, true);
    }

    void PiPiFieldManager::RestrictField(PdfObject* fieldObj) {
        PdfMemDocument* document = this->document;
        
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        // 取得爸爸
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        if (parentFieldObj == nullptr) {
            throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidRestrictFieldOperation);
        }
        
        PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
        
        // 處理父子間關係
        fieldDict->RemoveKey(PdfName("Parent"));
        parentFieldDict->RemoveKey(PdfName("Kids"));
        
        for (auto iterator = parentFieldDict->begin(); iterator != parentFieldDict->end(); iterator.operator++()) {
            const PdfName name = iterator->first;
            PdfObject* object = &(iterator->second);
            
            if (fieldDict->HasKey(name)) {
                continue;
            }
            
            fieldDict->AddKey(name, *object);
        }
        
        // 取得爺爺
        PdfObject* grandFieldObj = parentFieldDict->FindKey(PdfName("Parent"));
        PdfArray* grandFieldKids = nullptr;
        
        if (grandFieldObj == nullptr) {
            PdfAcroForm* acroform = document->GetAcroForm();
            PdfDictionary* acroformDict = &(acroform->GetDictionary());
            PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
            grandFieldKids = &(acroformFieldsObj->GetArray());
        } else {
            PdfDictionary* grandFieldDict = &(grandFieldObj->GetDictionary());
            PdfObject* grandKidsObj = grandFieldDict->FindKey(PdfName("Kids"));
            grandFieldKids = &(grandKidsObj->GetArray());
        }
        
        // 處理爺爸間關係
        for (unsigned int index = 0; index < grandFieldKids->size(); index++) {
            PdfObject* uncleFieldObj = grandFieldKids->FindAt(index);
            if (uncleFieldObj == parentFieldObj) {
                grandFieldKids->RemoveAt(index);
                break;
            }
        }
        
        // 處理爺孫間關係
        if (grandFieldObj != nullptr) {
            fieldDict->AddKeyIndirect(PdfName("Parent"), *grandFieldObj);
        }
        
        grandFieldKids->AddIndirect(*fieldObj);
    }

    void PiPiFieldManager::ExpandField(PdfObject* fieldObj) {
        PdfMemDocument* document = this->document;
        
        PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());
        
        PdfDictionary* fieldDict = &(fieldObj->GetDictionary());
        
        PdfObject* expandFieldObj = &(indirectObjectList->CreateDictionaryObject());
        PdfDictionary* expandFieldDict = &(expandFieldObj->GetDictionary());
        
        PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
        
        if (parentFieldObj != nullptr) {
            fieldDict->RemoveKey(PdfName("Parent"));
        }
        
        for (unsigned int i = 0; i < SpecialHierarchicalFieldKeys.size(); i++) {
            PdfName SpecialHierarchicalFieldKey = SpecialHierarchicalFieldKeys[i];
            
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

    void PiPiFieldManager::RemoveFieldMap(PdfObject *fieldObj, std::string fieldName) {
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        std::map<PdfObject*, PdfField*>* fieldBridgeMap = this->fieldBridgeMap;
        
        PdfField* field = this->BridgeObjectToField(fieldObj);
        auto findFieldMapIterator = fieldMap->find(fieldName);
        if (findFieldMapIterator != fieldMap->end()) {
            PiPiManagedFields* managedFields = findFieldMapIterator->second;
            managedFields->UnManageField(field);
        }
        
        auto findFieldBridgeMapIterator = fieldBridgeMap->find(fieldObj);
        if (findFieldBridgeMapIterator != fieldBridgeMap->end()) {
            fieldBridgeMap->erase(fieldObj);
        }
    }

    void PiPiFieldManager::RemoveAnnotationMap(PdfObject *annotObj, std::string fieldName) {
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        std::map<PdfObject*, PdfAnnotation*>* annotBridgeMap = this->annotBridgeMap;
        
        PdfAnnotation* annot = this->BridgeObjectToAnnotation(annotObj);
        
        auto findAnnotMapIterator = annotMap->find(fieldName);
        if (findAnnotMapIterator != annotMap->end()) {
            PiPiManagedAnnotations* managedAnnots = findAnnotMapIterator->second;
            managedAnnots->UnManageAnnotation(annot);
        }
        
        auto findAnnotBridgeMapIterator = annotBridgeMap->find(annotObj);
        if (findAnnotBridgeMapIterator != annotBridgeMap->end()) {
            annotBridgeMap->erase(findAnnotBridgeMapIterator);
        }
    }

    void PiPiFieldManager::AddFieldMap(PdfObject* fieldObj, PdfField* field, std::string fieldName, bool real) {
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        std::map<PdfObject*, PdfField*>* fieldBridgeMap = this->fieldBridgeMap;
        
        auto findFieldMapIterator = fieldMap->find(fieldName);
        if (findFieldMapIterator != fieldMap->end()) {
            PiPiManagedFields* managedFields = findFieldMapIterator->second;
            
            if (managedFields->IsReal() != real) {
                throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidRealStatus);
            }
            
            managedFields->ManageField(field);
        } else {
            PiPiManagedFields* managedFields = new PiPiManagedFields(field, real);
            fieldMap->insert(std::pair<const std::string, PiPiManagedFields*>(fieldName, managedFields));
        }
        
        fieldBridgeMap->insert(std::pair<PdfObject*, PdfField*>(fieldObj, field));
    }

    void PiPiFieldManager::AddAnnotationMap(PdfObject* annotObj, PdfAnnotation* annot, std::string fieldName) {
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        std::map<PdfObject*, PdfAnnotation*>* annotBridgeMap = this->annotBridgeMap;
        
        auto findAnnotMapIterator = annotMap->find(fieldName);
        if (findAnnotMapIterator != annotMap->end()) {
            PiPiManagedAnnotations* managedAnnots = findAnnotMapIterator->second;
            managedAnnots->ManageAnnotation(annot);
        } else {
            PiPiManagedAnnotations* managedAnnots = new PiPiManagedAnnotations(annot);
            annotMap->insert(std::pair<const std::string, PiPiManagedAnnotations*>(fieldName, managedAnnots));
        }
        
        annotBridgeMap->insert(std::pair<PdfObject*, PdfAnnotation*>(annotObj, annot));
    }

    void PiPiFieldManager::RenameFieldMap(std::string oldFieldName, std::string newFieldName) {
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        
        auto findIterator = fieldMap->find(oldFieldName);
        if (findIterator == fieldMap->end()) {
            return;
        }
        
        PiPiManagedFields* managedFields = findIterator->second;
        
        fieldMap->erase(findIterator);
        
        fieldMap->insert(std::pair<const std::string, PiPiManagedFields*>(newFieldName, managedFields));
    }

    void PiPiFieldManager::RenameAnnotationMap(std::string oldFieldName, std::string newFieldName) {
        std::map<const std::string, PiPiManagedAnnotations*>* annotMap = this->annotMap;
        
        auto findIterator = annotMap->find(oldFieldName);
        if (findIterator == annotMap->end()) {
            return;
        }
        
        PiPiManagedAnnotations* managedAnnots = findIterator->second;
        
        annotMap->erase(findIterator);
        
        annotMap->insert(std::pair<const std::string, PiPiManagedAnnotations*>(newFieldName, managedAnnots));
    }

    bool PiPiFieldManager::IsDuplicateFieldExists(std::string fieldName) {
        std::map<const std::string, PiPiManagedFields*>* fieldMap = this->fieldMap;
        
        for (auto iterator = fieldMap->begin(); iterator != fieldMap->end(); iterator.operator++()) {
            const std::string name = iterator->first;
            
            if (PiPiStringCommon::startsWith(name, fieldName) && name != fieldName) {
                return true;
            }
        }
        
        return false;
    }
}
