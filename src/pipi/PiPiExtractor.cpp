#include "PiPiExtractor.h"

namespace PiPi {
	PiPiExtractor::PiPiExtractor(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiExtractor::init(PdfMemDocument* document) {
		this->document = document;
	}

	std::vector<const PiPiPage*>* PiPiExtractor::extractPage() {
		std::vector<const PiPiPage*>* pages = new std::vector<const PiPiPage*>();

		PdfMemDocument* document = this->document;
		PdfPageCollection& pdfPages = document->GetPages();
		unsigned int pdfPageCount = pdfPages.GetCount();
		for (unsigned int pdfPageIndex = 0; pdfPageIndex < pdfPageCount; pdfPageIndex++) {
			PdfPage& pdfPage = pdfPages.GetPageAt(pdfPageIndex);

			Rect rect = pdfPage.GetRect();

			double bottom = rect.GetBottom();
			double top = rect.GetTop();
			double left = rect.GetLeft();
			double right = rect.GetRight();

			double width = right - left;
			double height = top - bottom;

			const PiPi::PiPiPage* page = new PiPi::PiPiPage(width, height);
			pages->push_back(page);
		}

		return pages;
	}
}