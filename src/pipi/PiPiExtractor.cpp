#include "PiPiExtractor.h"

namespace PiPi {
	PiPiExtractor::PiPiExtractor(PdfMemDocument* document, PiPiFieldManager* fieldManager) {
        this->document = document;
        this->fieldManager = fieldManager;
	}

	bool PiPiExtractor::IsOperable() {
		return this->document != nullptr;
	}

	std::vector<const PiPiPage*>* PiPiExtractor::ExtractPage() {
        PdfMemDocument* document = this->document;
        
		std::vector<const PiPiPage*>* pages = new std::vector<const PiPiPage*>();

		PdfPageCollection& pdfPages = document->GetPages();
		unsigned int pdfPageCount = pdfPages.GetCount();
		for (unsigned int pdfPageIndex = 0; pdfPageIndex < pdfPageCount; pdfPageIndex++) {
			PdfPage& pdfPageRef = pdfPages.GetPageAt(pdfPageIndex);
			PdfPage* pdfPage = &pdfPageRef;

			double width = PiPiExtractUtil::ExtractPageWidth(pdfPage);
			double height = PiPiExtractUtil::ExtractPageHeight(pdfPage);

			const PiPi::PiPiPage* page = new PiPi::PiPiPage(width, height);
			pages->push_back(page);
		}

		return pages;
	}

	std::vector<const PiPiField*>* PiPiExtractor::ExtractField() {
        PdfMemDocument* document = this->document;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
		std::vector<const PiPiField*>* tFields = new std::vector<const PiPiField*>();

        std::map<const std::string, std::set<PdfField*>*>* fieldMap = fieldManager->SearchAllField();
		for (auto fieldMapIterator = fieldMap->begin(); fieldMapIterator != fieldMap->end(); fieldMapIterator.operator++()) {

			std::string name = fieldMapIterator->first;
			std::set<PdfField*>* fields = fieldMapIterator->second;
			for (auto fieldIterator = fields->begin(); fieldIterator != fields->end(); fieldIterator.operator++()) {
                PdfField* field = *fieldIterator;
                PdfAnnotation* annot = fieldManager->BridgeFieldToAnnotation(field);

				PdfPage* page = annot->GetPage();
				unsigned int pageIndex = PiPiPageUtil::SearchPageIndex(document, page);

				double x = PiPiExtractUtil::ExtractAnnotationX(annot);
				double y = PiPiExtractUtil::ExtractAnnotationY(annot);
				double width = PiPiExtractUtil::ExtractAnnotationWidth(annot);
				double height = PiPiExtractUtil::ExtractAnnotationHeight(annot);

				std::string fontName = PiPiExtractUtil::ExtractAnnotationFontName(annot);
				float fontSize = PiPiExtractUtil::ExtractAnnotationFontSize(annot);

				PiPiFieldType type = PiPiExtractUtil::ExtractAnnotationType(annot);

				PiPiField* tField = new PiPiField(name, type, pageIndex, x, y, width, height, fontName, fontSize);

                tFields->push_back(tField);
			}
		}

		return tFields;
	}
}
