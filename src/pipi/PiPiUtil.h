#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiUtil {
		public:
			static std::map<const std::string, std::vector<const PdfField*>*>* SearchAllField(PdfMemDocument* document);
			static std::map<const std::string, std::vector<const PdfField*>*>* SearchAllField(PdfAcroForm* acroform);
			static std::vector<const PdfField*>* SearchField(PdfMemDocument* document, std::string fieldName);
			static std::vector<const PdfField*>* SearchField(PdfAcroForm* acroform, std::string fieldName);
			static std::vector<const PdfAnnotation*>* SearchAnnotation(PdfMemDocument* document, std::string fieldName);
			static std::vector<const PdfAnnotation*>* SearchAnnotation(PdfPageCollection* pages, std::string fieldName);
			static void RemoveField(PdfMemDocument* document, std::string fieldName);
		private:
			static void SearchAllChildrenField(PdfField* field, std::map<const std::string, std::vector<const PdfField*>*>* fieldMap);
			static void SearchChildrenField(PdfField* field, std::string fieldName, std::vector<const PdfField*>* fields);

			static void RemoveAcroformField(PdfAcroForm* acroform, std::string fieldName);
			static void RemoveAcroformChildrenField(PdfField* field, std::string fieldName);

			static void RemovePageField(PdfPageCollection* pages, std::string fieldName);
	};
}