#include "PiPiFieldManager.h"

namespace PiPi {
    PiPiFieldManager::PiPiFieldManager(PdfMemDocument* document) {
        this->document = document;
        
        this->fieldMap = new std::map<const std::string, std::set<PdfField*>*>();
        this->annotMap = new std::map<const std::string, std::set<PdfAnnotation*>*>();
        
        this->SearchAllField();
        this->SearchAllFieldAnnotation();
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
    }

    bool PiPiFieldManager::IsOperable() {
        return this->document != nullptr;
    }
    
    std::map<const std::string, std::set<PdfField*>*>* PiPiFieldManager::SearchAllField() {
        if (this->fieldMap == nullptr) {
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
            
            std::map<const std::string, std::set<PdfField*>*>* fieldMap = new std::map<const std::string, std::set<PdfField*>*>();
            for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
                this->SearchAllChildrenField(&(*iterator));
            }
            
            this->fieldMap = fieldMap;
        }
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = this->fieldMap;
        std::map<const std::string, std::set<PdfField*>*>* outputFieldMap = new std::map<const std::string, std::set<PdfField*>*>();
        for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::set<PdfField*>* fields = mapIterator->second;
            
            std::set<PdfField*>* outputFields = new std::set<PdfField*>();
            outputFields->insert(fields->begin(), fields->end());
            
            outputFieldMap->insert(std::pair<const std::string, std::set<PdfField*>*>(fieldName, outputFields));
        }
        
        return outputFieldMap;
    }
}
