#pragma once

#include "podofo/podofo.h"
#include "PiPiFieldType.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldManager {
        public:
            PiPiFieldManager(PdfMemDocument* document);
            ~PiPiFieldManager();
        
            bool IsOperable();
        
            std::map<const std::string, std::set<PdfField*>*>* SearchAllField();
            std::set<PdfField*>* SearchField(std::string fieldName);
        
            void RemoveAllField();
            void RemoveField(std::string fieldName);
            void RemoveField(std::string fieldName, long pageIndex);
            void RemoveField(std::string fieldName, long pageIndex, double x, double y);
            void RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height);
        
            void CreateField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
        
            void RenameField(std::string oldFieldName, std::string newFieldName);
        
            std::map<const std::string, std::set<PdfAnnotation*>*>* SearchAllFieldAnnotation();
            std::set<PdfAnnotation*>* SearchFieldAnnotation(std::string fieldName);
        
            void RemoveAllFieldAnnotation();
            void RemoveFieldAnnotation(std::string fieldName);
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex);
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y);
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y, double width, double height);
            
            void RenameFieldAnnotation(std::string oldFieldName, std::string newFieldName);
        
            PdfAnnotation* BridgeFieldToAnnotation(PdfField* field);
            PdfField* BridgeAnnotationToField(PdfAnnotation* annot);
        
        private:
            static const std::vector<PdfName> SpecialHierarchicalFieldKeys;
        
            PdfMemDocument* document;
            std::map<const std::string, std::set<PdfField*>*>* fieldMap;
            std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap;
        
            void SearchAllChildrenField(PdfObject* fieldObj);
        
            void RemoveChildrenField(PdfObject* fieldObj);
    };
}
