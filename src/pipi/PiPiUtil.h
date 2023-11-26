#pragma once

#include "podofo/podofo.h"
#include <vector>
#include <cstring>
#include "PiPiFieldType.h"
#include "PiPiCommon.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiUtil {
		public:
			static std::map<const std::string, std::vector<PdfField*>*>* SearchAllField(PdfMemDocument* document);
			static std::map<const std::string, std::vector<PdfField*>*>* SearchAllField(PdfAcroForm* acroform);

			static std::map<const std::string, std::vector<PdfAnnotation*>*>* SerachAllFieldAnnotation(PdfMemDocument* document);
			static std::map<const std::string, std::vector<PdfAnnotation*>*>* SerachAllFieldAnnotation(PdfPageCollection* pages);

			static std::vector<const PdfField*>* SearchField(PdfMemDocument* document, std::string fieldName);
			static std::vector<const PdfField*>* SearchField(PdfAcroForm* acroform, std::string fieldName);

			static std::vector<PdfAnnotation*>* SearchFieldAnnotation(PdfMemDocument* document, std::string fieldName);
			static std::vector<PdfAnnotation*>* SearchFieldAnnotation(PdfPageCollection* pages, std::string fieldName);

			static int SearchPageIndex(PdfMemDocument* document, PdfPage* page);
			static int SearchPageIndex(PdfPageCollection* pages, PdfPage* page);
        
            static void FlattenAnnotation(PdfAnnotation* annotation);

            static void RemoveAllField(PdfMemDocument* document);
			static void RemoveField(PdfMemDocument* document, std::string fieldName);

			static void CreateField(PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int page, double x, double y, double width, double height);
		private:
			static void SearchAllChildrenField(PdfField* field, std::map<const std::string, std::vector<PdfField*>*>* fieldMap);
			static void SearchChildrenField(PdfField* field, std::string fieldName, std::vector<const PdfField*>* fields);

            static void RemoveAllAcroformField(PdfAcroForm* acroform);
			static void RemoveAcroformField(PdfAcroForm* acroform, std::string fieldName);
			static void RemoveAcroformChildrenField(PdfField* field, std::string fieldName);

            static void RemoveAllPageField(PdfPageCollection* pages);
			static void RemovePageField(PdfPageCollection* pages, std::string fieldName);
	};
}
