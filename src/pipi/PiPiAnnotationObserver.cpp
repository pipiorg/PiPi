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

    void PiPiAnnotationObserver::observerAll(const std::map<const std::string, std::vector<PdfAnnotation *> *> * observedAnnotMap) {
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

    bool PiPiAnnotationObserver::access(const std::string fieldName, std::unique_ptr<std::vector<PdfAnnotation *>> annotsPtr) {
        if (!observed) {
            return false;
        }
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        std::vector<PdfAnnotation*>* outAnnots = new std::vector<PdfAnnotation*>();
        annotsPtr.reset(outAnnots);
        
        auto findIterator = annotMap->find(fieldName);
        if (findIterator == annotMap->end()) {
            return true;
        }
        
        std::vector<PdfAnnotation*>* annots = findIterator->second;
        outAnnots->assign(annots->begin(), annots->end());
        
        return true;
    }

    bool PiPiAnnotationObserver::access(const std::string fieldName, std::unique_ptr<std::map<const std::string, std::vector<PdfAnnotation*>*>> annotMapPtr) {
        if (!observed) {
            return false;
        }
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = this->annotMap;
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* outAnnotMap = new std::map<const std::string, std::vector<PdfAnnotation*>*>();
        annotMapPtr.reset(outAnnotMap);
        
        for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
            const std::string fieldName = mapIterator->first;
            std::vector<PdfAnnotation*>* annots = mapIterator->second;
            
            std::vector<PdfAnnotation*>* outAnnots = new std::vector<PdfAnnotation*>();
            outAnnots->assign(annots->begin(), annots->end());
            
            outAnnotMap->insert(std::pair<const std::string, std::vector<PdfAnnotation*>*>(fieldName, outAnnots));
        }
        
        return true;
    }
}
