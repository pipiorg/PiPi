#include "PiPiAnnotationObserver.h"

namespace PiPi {
    PiPiAnnotationObserver::PiPiAnnotationObserver() {
        this->observed = false;
        this->annotMap = new std::map<const std::string, std::set<PdfAnnotation*>*>();
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

    void PiPiAnnotationObserver::observeAll(const std::map<const std::string, std::set<PdfAnnotation *> *> * observedAnnotMap) {
        if (observed) {
            return;
        }
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        for (auto mapIterator = observedAnnotMap->begin(); mapIterator != observedAnnotMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::set<PdfAnnotation*>* observedAnnots = mapIterator->second;
            
            std::set<PdfAnnotation*>* annots = nullptr;
            auto findIterator = annotMap->find(fieldName);
            if (findIterator == annotMap->end()) {
                annots = new std::set<PdfAnnotation*>();
                annotMap->insert(std::pair<const std::string, std::set<PdfAnnotation*>*>(fieldName, annots));
            } else {
                annots = findIterator->second;
            }
            
            annots->insert(observedAnnots->begin(), observedAnnots->end());
        }
        
        observed = true;
    }

    bool PiPiAnnotationObserver::access(const std::string fieldName, std::set<PdfAnnotation *>** annotsPtr) {
        if (!observed) {
            return false;
        }
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        *annotsPtr = new std::set<PdfAnnotation*>();
        
        auto findIterator = annotMap->find(fieldName);
        if (findIterator == annotMap->end()) {
            return true;
        }
        
        std::set<PdfAnnotation*>* annots = findIterator->second;
        (*annotsPtr)->insert(annots->begin(), annots->end());
        
        return true;
    }

    bool PiPiAnnotationObserver::accessAll(std::map<const std::string, std::set<PdfAnnotation*>*>** annotMapPtr) {
        if (!observed) {
            return false;
        }
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        *annotMapPtr = new std::map<const std::string, std::set<PdfAnnotation*>*>();
        
        for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::set<PdfAnnotation*>* annots = mapIterator->second;
            
            std::set<PdfAnnotation*>* outAnnots = new std::set<PdfAnnotation*>();
            outAnnots->insert(annots->begin(), annots->end());
            
            (*annotMapPtr)->insert(std::pair<const std::string, std::set<PdfAnnotation*>*>(fieldName, outAnnots));
        }
        
        return true;
    }

    void PiPiAnnotationObserver::observeRemove(const std::string fieldName, PdfAnnotation *annot) {
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = this->annotMap;

        auto mapFindIterator = annotMap->find(fieldName);
        if (mapFindIterator == annotMap->end()) {
            return;
        }

        std::set<PdfAnnotation*>* annots = mapFindIterator->second;

        annots->erase(annot);
    }

    void PiPiAnnotationObserver::observeAdd(const std::string fieldName, PdfAnnotation *annot) {
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = this->annotMap;

        std::set<PdfAnnotation*>* annots = nullptr;

        auto mapFindIterator = annotMap->find(fieldName);
        if (mapFindIterator == annotMap->end()) {
            annots = new std::set<PdfAnnotation*>();

            std::pair<const std::string, std::set<PdfAnnotation*>*> annotPair(fieldName, annots);
            annotMap->insert(annotPair);
        }
        else {
            annots = mapFindIterator->second;
        }

        annots->insert(annot);
    }
}
