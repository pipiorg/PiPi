#include "PiPiExtractor.h"

namespace PiPi {
	PiPiExtractor::PiPiExtractor(PdfMemDocument* document, PiPiFieldManager* fieldManager) {
		this->operable = true;
        this->document = document;
        this->fieldManager = fieldManager;
	}

	bool PiPiExtractor::IsOperable() {
		spdlog::trace("IsOperable");

		return this->operable;
	}

	std::vector<const PiPiPage*>* PiPiExtractor::ExtractPage() {
		spdlog::trace("ExtractPage");

		if (!this->IsOperable()) {
			throw PiPiExtractException(PiPiExtractException::PiPiExtractExceptionCode::InOperable);
		}

        PdfMemDocument* document = this->document;
        
		std::vector<const PiPiPage*>* pages = new std::vector<const PiPiPage*>();

		PdfPageCollection& pdfPages = document->GetPages();
		unsigned int pdfPageCount = pdfPages.GetCount();
		for (unsigned int pdfPageIndex = 0; pdfPageIndex < pdfPageCount; pdfPageIndex++) {
			PdfPage& pdfPageRef = pdfPages.GetPageAt(pdfPageIndex);
			PdfPage* pdfPage = &pdfPageRef;

			double width = PiPiPageUtil::ExtractPageWidth(pdfPage);
			double height = PiPiPageUtil::ExtractPageHeight(pdfPage);

			const PiPi::PiPiPage* page = new PiPi::PiPiPage(width, height);
			pages->push_back(page);
		}

		return pages;
	}

	std::vector<const PiPiField*>* PiPiExtractor::ExtractField() {
		spdlog::trace("ExtractField");

		if (!this->IsOperable()) {
			throw PiPiExtractException(PiPiExtractException::PiPiExtractExceptionCode::InOperable);
		}

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

				double x = PiPiAnnotationUtil::ExtractAnnotationX(annot);
				double y = PiPiAnnotationUtil::ExtractAnnotationY(annot);
				double width = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
				double height = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

				std::string fontName = PiPiAnnotationUtil::ExtractAnnotationFontName(annot);
				float fontSize = PiPiAnnotationUtil::ExtractAnnotationFontSize(annot);

				PiPiFieldType type = PiPiAnnotationUtil::ExtractAnnotationType(annot);

				PiPiField* tField = new PiPiField(name, type, pageIndex, x, y, width, height, fontName, fontSize);

                tFields->push_back(tField);
			}
		}

		return tFields;
	}
}
