#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiAnnotationObserver {
        enum PiPiAnnotationObserveType {
            Add,
            Remove
        };
        
        public:
            PiPiAnnotationObserver();
            ~PiPiAnnotationObserver();
        
        void observe(PiPiAnnotationObserveType observeType, const std::string fieldName, PdfAnnotation* annot);
        void observeAll(const std::map<const std::string, std::vector<PdfAnnotation*>*>* observedMap);
        
        bool access(const std::string fieldName, std::unique_ptr<std::vector<PdfAnnotation*>> annotsPtr);
        bool access(const std::string fieldName, std::unique_ptr<std::map<const std::string, std::vector<PdfAnnotation*>*>> annotMapPtr);
        
        
        private:
            bool observed;
        
            std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap;
        
            void observeAdd(const std::string fieldName, PdfAnnotation* annot);
            void observeRemove(const std::string fieldName, PdfAnnotation* annot);
    };
}
