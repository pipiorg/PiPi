#pragma once

#include "podofo/podofo.h"
#include <vector>
#include <cstring>
#include "PiPiExtractUtil.h"
#include "PiPiAnnotationUtil.h"
#include "PiPiPageUtil.h"
#include "PiPiFieldObserver.h"
#include "PiPiAnnotationObserver.h"
#include "PiPiFieldType.h"
#include "PiPiCommon.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiFieldUtil {
		public:
            static std::set<PdfField*>* SearchField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string fieldName);
            static std::set<PdfField*>* SearchField(PiPiFieldObserver* fieldObserver, PdfAcroForm* acroform, std::string fieldName);

            static void RemoveAllField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document);
        
            static void RemoveField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName);
            static void RemoveField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, long pageIndex);
            static void RemoveField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, long pageIndex, double x, double y);
            static void RemoveField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, long pageIndex, double x, double y, double width, double height);
        
			static void CreateField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
        
            static void RenameField(PiPiFieldObserver* fieldObserver, PdfMemDocument* document, std::string oldFieldName, std::string newFieldName);
		private:
            static const std::vector<PdfName> SpecialHierarchicalFieldKeys;
        
			static void SearchAllChildrenField(PdfField* field, std::map<const std::string, std::set<PdfField*>*>* fieldMap);
        
			static void RemoveChildrenField(PiPiFieldObserver* fieldObserver, PdfDocument* document, PdfField* field);

            static void CreateNonChildrenField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
            static void CreateChildrenField(PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
        
            static void ExpandChildrenField(PdfDocument* document, std::string fieldName);
            static void RestrictChildrenField(PdfDocument* document, std::string fieldName);
	};
}
