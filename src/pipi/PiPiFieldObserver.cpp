#include "PiPiFieldObserver.h"

namespace PiPi {
    PiPiFieldObserver::PiPiFieldObserver() {
        this->observed = false;
        this->fieldMap = new std::map<const std::string, std::vector<PdfField*>*>();
    }

    PiPiFieldObserver::~PiPiFieldObserver() {
        for (auto iterator = this->fieldMap->begin(); iterator != this->fieldMap->end(); iterator.operator++()) {
            delete iterator->second;
            iterator->second = nullptr;
        }
        
        delete this->fieldMap;
        this->fieldMap = nullptr;
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

    void PiPiFieldObserver::observeAll(const std::map<const std::string, std::vector<PdfField *> *> *observedMap) {
        if (this->observed) {
            return;
        }
        
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = this->fieldMap;
        
        for (auto mapIterator = observedMap->begin(); mapIterator != observedMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::vector<PdfField*>* observedFields = mapIterator->second;
            
            std::vector<PdfField*>* fields = new std::vector<PdfField*>();
            fields->assign(observedFields->begin(), observedFields->end());
            
            fieldMap->insert(std::pair<const std::string, std::vector<PdfField*>*>(fieldName, fields));
        }
        
        this->observed = true;
    }

    bool PiPiFieldObserver::access(const std::string fieldName, std::unique_ptr<std::vector<PdfField *>> fieldsPtr) {
        if (!this->observed) {
            return false;
        }
        
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = this->fieldMap;
        
        std::vector<PdfField*>* outFields = new std::vector<PdfField*>();
        fieldsPtr.reset(outFields);
        
        auto findIterator = fieldMap->find(fieldName);
        if (findIterator == fieldMap->end()){
            return true;
        }
        
        std::vector<PdfField*>* fields = findIterator->second;
        outFields->assign(fields->begin(), fields->end());
        
        return true;
    }

    bool PiPiFieldObserver::access(const std::string fieldName, std::unique_ptr<std::map<const std::string, std::vector<PdfField *> *>> fieldMapPtr) {
        if (!this->observed) {
            return false;
        }
        
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = this->fieldMap;
        
        std::map<const std::string, std::vector<PdfField*>*>* outFieldMap = new std::map<const std::string, std::vector<PdfField*>*>();
        fieldMapPtr.reset(outFieldMap);
        
        for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::vector<PdfField*>* fields = mapIterator->second;
            
            std::vector<PdfField*>* outFields = new std::vector<PdfField*>();
            outFields->assign(fields->begin(), fields->end());
            
            outFieldMap->insert(std::pair<const std::string, std::vector<PdfField*>*>(fieldName, outFields));
        }
        
        return true;
    }

    void PiPiFieldObserver::observeAdd(const std::string fieldName, PdfField *field) {
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = this->fieldMap;
        
        std::vector<PdfField*>* fields = nullptr;
        
        auto mapFindIterator = fieldMap->find(fieldName);
        if (mapFindIterator == fieldMap->end()) {
            fields = new std::vector<PdfField*>();
            
            std::pair<const std::string, std::vector<PdfField*>*> fieldPair(fieldName, fields);
            fieldMap->insert(fieldPair);
        } else {
            fields = mapFindIterator->second;
        }
        
        auto findIterator = std::find(fields->begin(), fields->end(), field);
        if (findIterator == fields->end()) {
            fields->push_back(field);
        }
    }

    void PiPiFieldObserver::observeRemove(const std::string fieldName, PdfField *field) {
        std::map<const std::string, std::vector<PdfField*>*>* fieldMap = this->fieldMap;
        
        auto mapFindIterator = fieldMap->find(fieldName);
        if (mapFindIterator == fieldMap->end()) {
            return;
        }
        
        std::vector<PdfField*>* fields = mapFindIterator->second;
        
        fields->erase(std::remove(fields->begin(), fields->end(), field), fields->end());
    }
}
