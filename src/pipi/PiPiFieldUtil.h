#pragma once

#include "podofo/podofo.h"
#include <vector>
#include <cstring>
#include "PiPiExtractUtil.h"
#include "PiPiAnnotationUtil.h"
#include "PiPiFieldObserver.h"
#include "PiPiAnnotationObserver.h"
#include "PiPiFieldType.h"
#include "PiPiCommon.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiFieldUtil {
		public:
			static std::map<const std::string, std::set<PdfField*>*>* SearchAllField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document);
			static std::map<const std::string, std::set<PdfField*>*>* SearchAllField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform);

            static void RemoveAllField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document);
            static void RemoveField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string fieldName);
        
			static std::set<PdfField*>* SearchField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string fieldName);
			static std::set<PdfField*>* SearchField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform, std::string fieldName);
            
			static void CreateField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
		private:
            static const std::vector<PdfName> SpecialHierarchicalFieldKeys;
        
			static void SearchAllChildrenField(PdfField* field, std::map<const std::string, std::set<PdfField*>*>* fieldMap);
        
			static void RemoveChildrenField(PiPiFieldObserver* fieldObserver, PdfField* field, std::string fieldName);

            static void CreateNonHierarchicalField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
            static void CreateHierarchicalField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
        
            static void ExpandHierarchicalField(PdfDocument* document, std::string fieldName);
            static void RestrictHierarchicalField(PdfDocument* document, std::string fieldName);
	};
}
