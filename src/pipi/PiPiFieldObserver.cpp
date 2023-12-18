#include "PiPiFieldObserver.h"

namespace PiPi {
    PiPiFieldObserver::PiPiFieldObserver() {
        this->observed = false;
        this->fieldMap = new std::map<const std::string, std::set<PdfField*>*>();
    }

    PiPiFieldObserver::~PiPiFieldObserver() {
        for (auto iterator = this->fieldMap->begin(); iterator != this->fieldMap->end(); iterator.operator++()) {
            delete iterator->second;
            iterator->second = nullptr;
        }
        
        delete this->fieldMap;
        this->fieldMap = nullptr;
    }

    bool PiPiFieldObserver::isObserved() {
        return this->observed;
    }

    void PiPiFieldObserver::observe(PiPiFieldObserveType observeType, const std::string fieldName, PdfField *field) {
        switch (observeType) {
            case PiPiFieldObserveType::Add:
                this->observeAdd(fieldName, field);
                return;
            case PiPiFieldObserveType::Remove:
                this->observeRemove(fieldName, field);
                return;
        }
    }

    void PiPiFieldObserver::observeAll(const std::map<const std::string, std::set<PdfField *> *> *observedMap) {
        if (this->observed) {
            return;
        }
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = this->fieldMap;
        
        for (auto mapIterator = observedMap->begin(); mapIterator != observedMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::set<PdfField*>* observedFields = mapIterator->second;
            
            std::set<PdfField*>* fields = nullptr;
            
            auto findIterator = fieldMap->find(fieldName);
            if (findIterator == fieldMap->end()) {
                fields = new std::set<PdfField*>();
                fieldMap->insert(std::pair<const std::string, std::set<PdfField*>*>(fieldName, fields));
            } else {
                fields = findIterator->second;
            }
            
            fields->insert(observedFields->begin(), observedFields->end());
        }
        
        this->observed = true;
    }

    bool PiPiFieldObserver::access(const std::string fieldName, std::set<PdfField *>** fieldsPtr) {
        if (!this->observed) {
            return false;
        }
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = this->fieldMap;
        
        *fieldsPtr = new std::set<PdfField*>();
        
        auto findIterator = fieldMap->find(fieldName);
        if (findIterator == fieldMap->end()){
            return true;
        }
        
        std::set<PdfField*>* fields = findIterator->second;
        (*fieldsPtr)->insert(fields->begin(), fields->end());
        
        return true;
    }

    bool PiPiFieldObserver::accessAll(std::map<const std::string, std::set<PdfField *> *>** fieldMapPtr) {
        if (!this->observed) {
            return false;
        }
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = this->fieldMap;
        
        *fieldMapPtr = new std::map<const std::string, std::set<PdfField*>*>();
        
        for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::set<PdfField*>* fields = mapIterator->second;
            
            std::set<PdfField*>* outFields = new std::set<PdfField*>();
            outFields->insert(fields->begin(), fields->end());
            
            (*fieldMapPtr)->insert(std::pair<const std::string, std::set<PdfField*>*>(fieldName, outFields));
        }
        
        return true;
    }

    void PiPiFieldObserver::observeAdd(const std::string fieldName, PdfField *field) {
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = this->fieldMap;
        
        std::set<PdfField*>* fields = nullptr;
        
        auto mapFindIterator = fieldMap->find(fieldName);
        if (mapFindIterator == fieldMap->end()) {
            fields = new std::set<PdfField*>();
            
            std::pair<const std::string, std::set<PdfField*>*> fieldPair(fieldName, fields);
            fieldMap->insert(fieldPair);
        } else {
            fields = mapFindIterator->second;
        }
        
        fields->insert(field);
    }

    void PiPiFieldObserver::observeRemove(const std::string fieldName, PdfField *field) {
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = this->fieldMap;
        
        auto mapFindIterator = fieldMap->find(fieldName);
        if (mapFindIterator == fieldMap->end()) {
            return;
        }
        
        std::set<PdfField*>* fields = mapFindIterator->second;
        
        fields->erase(field);
    }
}
