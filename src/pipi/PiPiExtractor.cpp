#include "PiPiExtractor.h"

namespace PiPi {
	PiPiExtractor::PiPiExtractor(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiExtractor::init(PdfMemDocument* document) {
		this->document = document;
	}

	bool PiPiExtractor::isOperable() {
		return this->document != nullptr;
	}

	std::vector<const PiPiPage*>* PiPiExtractor::extractPage() {
		std::vector<const PiPiPage*>* pages = new std::vector<const PiPiPage*>();

		PdfMemDocument* document = this->document;
		PdfPageCollection& pdfPages = document->GetPages();
		unsigned int pdfPageCount = pdfPages.GetCount();
		for (unsigned int pdfPageIndex = 0; pdfPageIndex < pdfPageCount; pdfPageIndex++) {
			PdfPage& pdfPageRef = pdfPages.GetPageAt(pdfPageIndex);
			PdfPage* pdfPage = &pdfPageRef;

			double width = PiPiUtil::ExtractPageWidth(pdfPage);
			double height = PiPiUtil::ExtractPageHeight(pdfPage);

			const PiPi::PiPiPage* page = new PiPi::PiPiPage(width, height);
			pages->push_back(page);
		}

		return pages;
	}

	std::vector<const PiPiField*>* PiPiExtractor::extractField() {
		std::vector<const PiPiField*>* fields = new std::vector<const PiPiField*>();

		PdfMemDocument* document = this->document;
		std::map<const std::string, std::vector<const PdfAnnotation*>*>* annotMap = PiPiUtil::SerachAllAnnotation(document);
		for (auto annotMapIterator = annotMap->begin(); annotMapIterator != annotMap->end(); annotMapIterator.operator++()) {
			std::pair<const std::string, std::vector<const PdfAnnotation*>*> pair = annotMapIterator.operator*();

			std::string name = pair.first;
			std::vector<const PdfAnnotation*>* annots = pair.second;
			for (auto annotIterator = annots->begin(); annotIterator != annots->end(); annotIterator.operator++()) {
				const PdfAnnotation* constAnnot = annotIterator.operator*();
				PdfAnnotation* annot = const_cast<PdfAnnotation*>(constAnnot);

				const PdfPage* constPage = annot->GetPage();
				PdfPage* page = const_cast<PdfPage*>(constPage);
				unsigned int pageIndex = PiPiUtil::SearchPageIndex(document, page);

				double x = PiPiUtil::ExtractAnnotationX(annot);
				double y = PiPiUtil::ExtractAnnotationY(annot);
				double width = PiPiUtil::ExtractAnnotationWidth(annot);
				double height = PiPiUtil::ExtractAnnotationHeight(annot);

				std::string font = PiPiUtil::ExtractAnnotationFont(annot);
				float fontSize = PiPiUtil::ExtractAnnotationFontSize(annot);

				PiPiFieldType type = PiPiUtil::ExtractAnnotationType(annot);

				PiPiField* field = new PiPiField(name, type, pageIndex, x, y, width, height, font, fontSize);

				fields->push_back(field);
			}
		}

		return fields;
	}
}