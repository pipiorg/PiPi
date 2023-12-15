#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldObserver {
        public:
            enum PiPiFieldObserveType {
                Add,
                Remove
            };
        
            PiPiFieldObserver();
            ~PiPiFieldObserver();
        
            bool isObserved();
        
            void observe(PiPiFieldObserveType observeType, const std::string fieldName, PdfField* field);
            void observeAll(const std::map<const std::string, std::vector<PdfField*>*>* observedMap);
            
            bool access(const std::string fieldName, std::vector<PdfField*>** fieldsPtr);
            bool accessAll(std::map<const std::string, std::vector<PdfField*>*>** fieldMapPtr);

        private:
            std::map<const std::string, std::vector<PdfField*>*>* fieldMap;
        
            bool observed;
        
            void observeAdd(const std::string fieldName, PdfField* field);
            void observeRemove(const std::string fieldName, PdfField* field);
    };
}
