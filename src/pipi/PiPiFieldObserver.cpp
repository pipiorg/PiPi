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

    void PiPiFieldObserver::observer(const std::map<const std::string, std::vector<PdfField *> *> *observedMap) {
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
}
