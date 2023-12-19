#pragma once

#include "podofo/podofo.h"
#include "PiPiAnnotationObserver.h"
#include "PiPiExtractUtil.h"
#include "PiPiPageUtil.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiAnnotationUtil {
        public:
            static std::map<const std::string, std::set<PdfAnnotation*>*>* SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document);
            static std::map<const std::string, std::set<PdfAnnotation*>*>* SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages);
            
            static std::set<PdfAnnotation*>* SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName);
            static std::set<PdfAnnotation*>* SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName);
        
            static void RemoveAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document);
            
            static void RemoveFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName);
            static void RemoveFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, long pageIndex);
            static void RemoveFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, long pageIndex, double x, double y);
            static void RemoveFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, long pageIndex, double x, double y, double width, double height);
            
            static void RenameFieldAnnotation(PiPiAnnotationObserver* annotObserver, std::string oldFieldName, std::string newFieldName);
        
            static void FlattenAnnotation(PdfAnnotation* annotation);
        };
}
