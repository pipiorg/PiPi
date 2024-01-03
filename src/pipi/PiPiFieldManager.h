#pragma once

#include "podofo/podofo.h"
#include "PiPiFieldType.h"
#include "PiPiManagedFields.h"
#include "PiPiManagedAnnotations.h"
#include "PiPiManageFieldException.h"
#include "PiPiPageUtil.h"
#include "PiPiExtractUtil.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiFieldManager {
        public:
            PiPiFieldManager(PdfMemDocument* document);
            ~PiPiFieldManager();
        
            bool IsOperable();
        
            std::map<const std::string, std::set<PdfField*>*>* SearchAllField(); //V
            std::set<PdfField*>* SearchField(std::string fieldName); //V
        
            void RemoveAllField(); //V
            void RemoveField(std::string fieldName); // V
            void RemoveField(std::string fieldName, long pageIndex); //V
            void RemoveField(std::string fieldName, long pageIndex, double x, double y); //V
            void RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height); //V
        
            PdfField* CreateField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
        
            void RenameField(std::string oldFieldName, std::string newFieldName);
        
            std::map<const std::string, std::set<PdfAnnotation*>*>* SearchAllFieldAnnotation();//V
            std::set<PdfAnnotation*>* SearchFieldAnnotation(std::string fieldName);//V
        
            void RemoveAllFieldAnnotation(); //V
            void RemoveFieldAnnotation(std::string fieldName); //V
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex); //V
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y); //V
            void RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y, double width, double height); //V
            
            PdfAnnotation* CreateFieldAnnotation(std::string fieldName, unsigned int pageIndex, double x, double y, double width, double height); //V
        
            void RenameFieldAnnotation(std::string oldFieldName, std::string newFieldName);
        
            PdfAnnotation* BridgeFieldToAnnotation(PdfField* field); //V
            PdfAnnotation* BridgeObjectToAnnotation(PdfObject* object);// V
        
            PdfField* BridgeAnnotationToField(PdfAnnotation* annot); //V
            PdfField* BridgeObjectToField(PdfObject* object);//V
        
        private:
            static const std::vector<PdfName> SpecialHierarchicalFieldKeys; // V
        
            PdfMemDocument* document;
            std::map<const std::string, PiPiManagedFields*>* fieldMap;
            std::map<const std::string, PiPiManagedAnnotations*>* annotMap;
            std::map<PdfObject*, PdfAnnotation*>* annotBridgeMap;
            std::map<PdfObject*, PdfField*>* fieldBridgeMap;
        
            void InitFieldMap(); // V
            void InitAnnotationMap(); // V
            void InitFieldBridgeMap();// V
            void InitAnnotationBridgeMap(); //V
        
            void InnerSearchAllField(PdfObject* fieldObj); // V
        
            void InnerRemoveField(PdfObject* fieldObj);//V
            void InnerRemoveFieldFromAcroform(PdfObject* fieldObj);//V
            void InnerRemoveFieldFromParentField(PdfObject* fieldObj);//V
        
            PdfObject* InnerCreateRealField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height); //V
            PdfObject* InnerCreateFakeField(std::string fieldName); //V
        
            void RestrictField(PdfObject* fieldObj); //V
        
            void ExpandField(PdfObject* fieldObj); //v
        
            void RemoveFieldMap(PdfObject* fieldObj, std::string fieldName);//V
            void RemoveAnnotationMap(PdfObject* annotObj, std::string fieldName);//V
        
            void RenameFieldMap(std::string oldFieldName, std::string newFieldName);
            void RenameAnnotationMap(std::string oldFieldName, std::string newFieldName);
        
            void AddFieldMap(PdfObject* fieldObj, PdfField* field, std::string fieldName, bool real); //V
            void AddAnnotationMap(PdfObject* annotObj, PdfAnnotation* annot, std::string fieldName); //V
        
            bool IsDuplicateFieldExists(std::string fieldName);
    };
}
