#include "PiPiOperator.h"

namespace PiPi {
	PiPiOperator::PiPiOperator(char* pdfBytes, size_t pdfSize) {
		this->init(pdfBytes, pdfSize);
	}

	void PiPiOperator::init(char* pdfBytes, size_t pdfSize) {
		PoDoFo::PdfMemDocument * document = new PoDoFo::PdfMemDocument();
		auto input = make_shared<PoDoFo::SpanStreamDevice>(pdfBytes, pdfSize);
		document->LoadFromDevice(input);
		
		this->document = document;

		this->pager = nullptr;
		this->editor = nullptr;
		this->filler = nullptr;
		this->extractor = nullptr;
		this->fontManager = nullptr;
	}

	PiPiFiller* PiPiOperator::getFiller() {
		if (this->filler != nullptr) {
			return this->filler;
		}

		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = getFontManager();
		PiPiFiller* filler = new PiPiFiller(document, fontManager);

		this->filler = filler;

		return this->filler;
	}

	PiPiPager* PiPiOperator::getPager() {
		if (this->pager != nullptr) {
			return this->pager;
		}

		PdfMemDocument* document = this->document;
		PiPiPager* pager = new PiPiPager(document);

		this->pager = pager;

		return this->pager;
	}

	PiPiEditor* PiPiOperator::getEditor() {
		if (this->editor != nullptr) {
			return this->editor;
		}

		PdfMemDocument* document = this->document;
		PiPiEditor* editor = new PiPiEditor(document);

		this->editor = editor;

		return this->editor;
	}

	PiPiExtractor* PiPiOperator::getExtractor() {
		if (this->extractor != nullptr) {
			return this->extractor;
		}

		PdfMemDocument* document = this->document;
		PiPiExtractor* extractor = new PiPiExtractor(document);

		this->extractor = extractor;

		return this->extractor;
	}

	PiPiFontManager* PiPiOperator::getFontManager() {
		if (this->fontManager != nullptr) {
			return this->fontManager;
		}
		
		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = new PiPiFontManager(document);

		this->fontManager = fontManager;
		
		return this->fontManager;
	}

	bool PiPiOperator::isOperable() {
		return this->document != nullptr;
	}

	void PiPiOperator::finalize(char** newPdfBytes, size_t* newPdfSize) {
		vector<char> outputVector;
		PoDoFo::VectorStreamDevice outputStreamDevice(outputVector);
		this->document->Save(outputStreamDevice);

		*newPdfSize = outputVector.size();
		*newPdfBytes = new char[*newPdfSize];
		for (size_t i = 0; i < *newPdfSize; i++) {
			*(*newPdfBytes + i) = outputVector[i];
		}

		delete this->fontManager;
		delete this->document;
		this->document = nullptr;
		this->fontManager = nullptr;
	}
}