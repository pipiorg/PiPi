#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiAnnotationObserver {
        public:
            PiPiAnnotationObserver();
            ~PiPiAnnotationObserver();
        
        void observerAll(const std::map<const std::string, std::vector<PdfAnnotation*>*>* observedMap);
        
        bool access(const std::string fieldName, std::unique_ptr<std::vector<PdfAnnotation*>> annotsPtr);
        bool access(const std::string fieldName, std::unique_ptr<std::map<const std::string, std::vector<PdfAnnotation*>*>> annotMapPtr);
        
        
        private:
            bool observed;
        
            std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap;
    };
}
