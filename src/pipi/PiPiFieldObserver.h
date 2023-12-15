#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldObserver {
        public:
            PiPiFieldObserver();
            ~PiPiFieldObserver();
        
        void observerAll(const std::map<const std::string, std::vector<PdfField*>*>* observedMap);
        
        bool access(const std::string fieldName, std::unique_ptr<std::vector<PdfField*>> fieldsPtr);
        bool access(const std::string fieldName, std::unique_ptr<std::map<const std::string, std::vector<PdfField*>*>> fieldMapPtr);

        private:
            std::map<const std::string, std::vector<PdfField*>*>* fieldMap;
        
            bool observed;
    };
}
