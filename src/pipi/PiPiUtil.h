#pragma once

#include "podofo/podofo.h"
#include <vector>
#include <cstring>
#include "PiPiFieldType.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiUtil {
		public:
			static std::map<const std::string, std::vector<const PdfField*>*>* SearchAllField(PdfMemDocument* document);
			static std::map<const std::string, std::vector<const PdfField*>*>* SearchAllField(PdfAcroForm* acroform);

			static std::map<const std::string, std::vector<const PdfAnnotation*>*>* SerachAllFieldAnnotation(PdfMemDocument* document);
			static std::map<const std::string, std::vector<const PdfAnnotation*>*>* SerachAllFieldAnnotation(PdfPageCollection* pages);

			static const PdfField* SearchField(PdfMemDocument* document, std::string fieldName);
			static const PdfField* SearchField(PdfAcroForm* acroform, std::string fieldName);

			static std::vector<const PdfAnnotation*>* SearchFieldAnnotation(PdfMemDocument* document, std::string fieldName);
			static std::vector<const PdfAnnotation*>* SearchFieldAnnotation(PdfPageCollection* pages, std::string fieldName);

			static int SearchPageIndex(PdfMemDocument* document, PdfPage* page);
			static int SearchPageIndex(PdfPageCollection* pages, PdfPage* page);

			static double ExtractPageWidth(PdfPage* page);
			static double ExtractPageHeight(PdfPage* page);

			static double ExtractPageX(PdfPage* page);
			static double ExtractPageY(PdfPage* page);

			static std::string ExtractAnnotationFontName(PdfAnnotation* annotation);
			static float ExtractAnnotationFontSize(PdfAnnotation* annotation);
			
			static double ExtractAnnotationWidth(PdfAnnotation* annotation);
			static double ExtractAnnotationHeight(PdfAnnotation* annotation);

			static double ExtractAnnotationX(PdfAnnotation* annotation);
			static double ExtractAnnotationY(PdfAnnotation* annotation);

			static PiPiFieldType ExtractAnnotationType(PdfAnnotation* annotation);
        
            static void FlattenAnnotation(PdfAnnotation* annotation);

            static void RemoveAllField(PdfMemDocument* document);
			static void RemoveField(PdfMemDocument* document, std::string fieldName);
		private:
			static void SearchAllChildrenField(PdfField* field, std::map<const std::string, std::vector<const PdfField*>*>* fieldMap);
			static const PdfField* SearchChildrenField(PdfField* field, std::string fieldName);

            static void RemoveAllAcroformField(PdfAcroForm* acroform);
			static void RemoveAcroformField(PdfAcroForm* acroform, std::string fieldName);
			static void RemoveAcroformChildrenField(PdfField* field, std::string fieldName);

            static void RemoveAllPageField(PdfPageCollection* pages);
			static void RemovePageField(PdfPageCollection* pages, std::string fieldName);

			static std::vector<std::string>* split(const std::string& str, const std::string& pattern);
	};
}
