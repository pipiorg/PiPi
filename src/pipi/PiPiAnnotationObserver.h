#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiAnnotationObserver {
        public:
            enum PiPiAnnotationObserveType {
                Add,
                Remove,
                Clear
            };
        
            PiPiAnnotationObserver();
            ~PiPiAnnotationObserver();
        
            bool isObserved();
        
            void observe(PiPiAnnotationObserveType observeType, const std::string fieldName, PdfAnnotation* annot);
            void observeAll(const std::map<const std::string, std::set<PdfAnnotation*>*>* observedMap);
            
            bool access(const std::string fieldName, std::set<PdfAnnotation*>** annotsPtr);
            bool accessAll(std::map<const std::string, std::set<PdfAnnotation*>*>** annotMapPtr);
        
        
        private:
            bool observed;
        
            std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap;
        
            void observeAdd(const std::string fieldName, PdfAnnotation* annot);
            void observeRemove(const std::string fieldName, PdfAnnotation* annot);
            void observeClear();
    };
}
