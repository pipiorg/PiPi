#include "PiPiAnnotationObserver.h"

namespace PiPi {
    PiPiAnnotationObserver::PiPiAnnotationObserver() {
        this->observed = false;
        this->annotMap = new std::map<const std::string, std::vector<PdfAnnotation*>*>();
    }

    PiPiAnnotationObserver::~PiPiAnnotationObserver() {
        for (auto iterator = this->annotMap->begin(); iterator != this->annotMap->end(); iterator.operator++()) {
            delete iterator->second;
            iterator->second = nullptr;
        }
        
        delete this->annotMap;
        this->annotMap = nullptr;
    }

    bool PiPiAnnotationObserver::isObserved() {
        return this->observed;
    }

    void PiPiAnnotationObserver::observe(PiPiAnnotationObserveType observeType, const std::string fieldName, PdfAnnotation *annot) {
        switch (observeType) {
            case PiPiAnnotationObserveType::Add:
                this->observeAdd(fieldName, annot);
                return;
            case PiPiAnnotationObserveType::Remove:
                this->observeRemove(fieldName, annot);
                return;
        }
    }

    void PiPiAnnotationObserver::observeAll(const std::map<const std::string, std::vector<PdfAnnotation *> *> * observedAnnotMap) {
        if (observed) {
            return;
        }
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        for (auto mapIterator = observedAnnotMap->begin(); mapIterator != observedAnnotMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::vector<PdfAnnotation*>* observedAnnots = mapIterator->second;
            
            std::vector<PdfAnnotation*>* annots = new std::vector<PdfAnnotation*>();
            annots->assign(observedAnnots->begin(), observedAnnots->end());
            
            annotMap->insert(std::pair<const std::string, std::vector<PdfAnnotation*>*>(fieldName, annots));
        }
        
        observed = true;
    }

    bool PiPiAnnotationObserver::access(const std::string fieldName, std::vector<PdfAnnotation *>** annotsPtr) {
        if (!observed) {
            return false;
        }
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        *annotsPtr = new std::vector<PdfAnnotation*>();
        
        auto findIterator = annotMap->find(fieldName);
        if (findIterator == annotMap->end()) {
            return true;
        }
        
        std::vector<PdfAnnotation*>* annots = findIterator->second;
        (*annotsPtr)->assign(annots->begin(), annots->end());
        
        return true;
    }

    bool PiPiAnnotationObserver::accessAll(std::map<const std::string, std::vector<PdfAnnotation*>*>** annotMapPtr) {
        if (!observed) {
            return false;
        }
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        *annotMapPtr = new std::map<const std::string, std::vector<PdfAnnotation*>*>();
        
        for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::vector<PdfAnnotation*>* annots = mapIterator->second;
            
            std::vector<PdfAnnotation*>* outAnnots = new std::vector<PdfAnnotation*>();
            outAnnots->assign(annots->begin(), annots->end());
            
            (*annotMapPtr)->insert(std::pair<const std::string, std::vector<PdfAnnotation*>*>(fieldName, outAnnots));
        }
        
        return true;
    }

    void PiPiAnnotationObserver::observeRemove(const std::string fieldName, PdfAnnotation *annot) {
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        std::vector<PdfAnnotation*>* annots = nullptr;
        
        auto mapFindIterator = annotMap->find(fieldName);
        if (mapFindIterator == annotMap->end()) {
            annots = new std::vector<PdfAnnotation*>();
            
            std::pair<const std::string, std::vector<PdfAnnotation*>*> annotPair(fieldName, annots);
            annotMap->insert(annotPair);
        } else {
            annots = mapFindIterator->second;
        }
        
        auto findIterator = std::find(annots->begin(), annots->end(), annot);
        if (findIterator == annots->end()) {
            annots->push_back(annot);
        }
    }

    void PiPiAnnotationObserver::observeAdd(const std::string fieldName, PdfAnnotation *annot) {
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        auto mapFindIterator = annotMap->find(fieldName);
        if (mapFindIterator == annotMap->end()) {
            return;
        }
        
        std::vector<PdfAnnotation*>* annots = mapFindIterator->second;
        
        annots->erase(std::remove(annots->begin(), annots->end(), annot), annots->end());
    }
}
