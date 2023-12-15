#pragma once

#include "podofo/podofo.h"
#include <vector>
#include <cstring>
#include "PiPiExtractUtil.h"
#include "PiPiFieldObserver.h"
#include "PiPiAnnotationObserver.h"
#include "PiPiFieldType.h"
#include "PiPiCommon.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiFieldUtil {
		public:
			static std::map<const std::string, std::vector<PdfField*>*>* SearchAllField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document);
			static std::map<const std::string, std::vector<PdfField*>*>* SearchAllField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform);

			static std::map<const std::string, std::vector<PdfAnnotation*>*>* SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document);
			static std::map<const std::string, std::vector<PdfAnnotation*>*>* SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages);

			static std::vector<PdfField*>* SearchField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string fieldName);
			static std::vector<PdfField*>* SearchField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform, std::string fieldName);

			static std::vector<PdfAnnotation*>* SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName);
			static std::vector<PdfAnnotation*>* SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName);

			static int SearchPageIndex(PdfMemDocument* document, PdfPage* page);
			static int SearchPageIndex(PdfPageCollection* pages, PdfPage* page);
        
            static void FlattenAnnotation(PdfAnnotation* annotation);

            static void RemoveAllField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document);
			static void RemoveField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName);

			static void CreateField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int page, double x, double y, double width, double height);
		private:
			static void SearchAllChildrenField(PdfField* field, std::map<const std::string, std::vector<PdfField*>*>* fieldMap);
			static void SearchChildrenField(PdfField* field, std::string fieldName, std::vector<PdfField*>* fields);

            static void RemoveAllAcroformField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform);
			static void RemoveAcroformField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform, std::string fieldName);
			static void RemoveAcroformChildrenField(PiPiFieldObserver* fieldObserver, PdfField* field, std::string fieldName);

            static void RemoveAllPageField(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages);
			static void RemovePageField(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName);
	};
}
