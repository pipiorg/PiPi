#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldObserver {
        public:
            PiPiFieldObserver();
            ~PiPiFieldObserver();
        
            bool isObserved();
        
            void observeAdd(const std::string fieldName, PdfField* field);
            void observeRemove(const std::string fieldName, PdfField* field);
            void observeRename(const std::string oldFieldName, const std::string newFieldName);
            void observeClear();
        
            void observeAll(const std::map<const std::string, std::set<PdfField*>*>* observedMap);
            
            bool access(const std::string fieldName, std::set<PdfField*>** fieldsPtr);
            bool accessAll(std::map<const std::string, std::set<PdfField*>*>** fieldMapPtr);

        private:
            std::map<const std::string, std::set<PdfField*>*>* fieldMap;
        
            bool observed;
    };
}
