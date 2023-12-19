#pragma once

#include "podofo/podofo.h"
#include "PiPiAnnotationObserver.h"
#include "PiPiExtractUtil.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiAnnotationUtil {
        public:
            static std::map<const std::string, std::set<PdfAnnotation*>*>* SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document);
            static std::map<const std::string, std::set<PdfAnnotation*>*>* SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages);
            
            static std::set<PdfAnnotation*>* SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName);
            static std::set<PdfAnnotation*>* SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName);
        
            static void RemoveAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document);
            static void RemoveAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages);
            
            static void RemoveFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName);
            static void RemoveFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName);
            
            static void FlattenAnnotation(PdfAnnotation* annotation);
        };
}
