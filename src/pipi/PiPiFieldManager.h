#pragma once

#include "podofo/podofo.h"
#include "PiPiFieldType.h"
#include "PiPiManagedFields.h"
#include "PiPiManagedAnnotations.h"
#include "PiPiManageFieldException.h"
#include "PiPiPageUtil.h"
#include "PiPiAnnotationUtil.h"
#include "PiPiFieldCompatibilityUtil.h"
#include "PiPiFieldConstants.h"

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
        
            PdfField* CreateField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
        
            void RenameField(std::string oldFieldName, std::string newFieldName);
        
            std::map<const std::string, std::set<PdfAnnotation*>*>* SearchAllFieldAnnotation();
            std::set<PdfAnnotation*>* SearchFieldAnnotation(std::string fieldName);
        
            void RemoveAllFieldAnnotation();
            void RemoveFieldAnnotation(std::string fieldName);
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex);
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y);
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y, double width, double height);
            
            PdfAnnotation* CreateFieldAnnotation(std::string fieldName, unsigned int pageIndex, double x, double y, double width, double height);
        
            void RenameFieldAnnotation(std::string oldFieldName, std::string newFieldName);
        
            PdfAnnotation* BridgeFieldToAnnotation(PdfField* field);
            PdfAnnotation* BridgeObjectToAnnotation(PdfObject* object);
        
            PdfField* BridgeAnnotationToField(PdfAnnotation* annot);
            PdfField* BridgeObjectToField(PdfObject* object);
        
        private:
            PdfMemDocument* document;
            std::map<const std::string, PiPiManagedFields*>* fieldMap;
            std::map<const std::string, PiPiManagedAnnotations*>* annotMap;
            std::map<PdfObject*, PdfAnnotation*>* annotBridgeMap;
            std::map<PdfObject*, PdfField*>* fieldBridgeMap;
        
            void InitDocument(PdfMemDocument* document);
            void InitFieldMap();
            void InitAnnotationMap();
            void InitFieldBridgeMap();
            void InitAnnotationBridgeMap();
        
            void InnerSearchAllField(PdfObject* fieldObj);
        
            void InnerRemoveField(PdfObject* fieldObj);
            void InnerRemoveFieldFromAcroform(PdfObject* fieldObj);
            void InnerRemoveFieldFromParentField(PdfObject* fieldObj);
        
            PdfObject* InnerCreateRealField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
            PdfObject* InnerCreateFakeField(std::string fieldName);
        
            void RestrictField(PdfObject* fieldObj);
        
            void ExpandField(PdfObject* fieldObj);
        
            void RemoveFieldMap(PdfObject* fieldObj, std::string fieldName);
            void RemoveAnnotationMap(PdfObject* annotObj, std::string fieldName);
        
            void RenameFieldMap(std::map<PdfObject*, PdfField*>* renameFieldMap, std::string fieldName);
            void RenameAnnotationMap(std::map<PdfObject*, PdfAnnotation*>* renameAnnotMap, std::string fieldName);
        
            void AddFieldMap(PdfObject* fieldObj, PdfField* field, std::string fieldName, bool real);
            void AddAnnotationMap(PdfObject* annotObj, PdfAnnotation* annot, std::string fieldName);
        
            bool IsDuplicateFieldExists(std::string fieldName);
    };
}
