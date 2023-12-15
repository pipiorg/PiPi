#include "PiPiOperator.h"

namespace PiPi {
	PiPiOperator::PiPiOperator(char* pdfBytes, size_t pdfSize) {
		this->init(pdfBytes, pdfSize);
	}

    PiPiOperator::~PiPiOperator() {
        if (this->annotObserver != nullptr) {
            delete this->annotObserver;
            this->annotObserver = nullptr;
        }
        
        if (this->fieldObserver != nullptr) {
            delete this->fieldObserver;
            this->fieldObserver = nullptr;
        }
        
        if (this->fontManager != nullptr) {
            delete this->fontManager;
            this->fontManager = nullptr;
        }
        
        if (this->pager != nullptr) {
            delete this->pager;
            this->pager = nullptr;
        }
        
        if (this->editor != nullptr) {
            delete this->editor;
            this->editor = nullptr;
        }
        
        if (this->filler != nullptr) {
            delete this->filler;
            this->filler = nullptr;
        }
        
        if (this->extractor != nullptr) {
            delete this->extractor;
            this->extractor = nullptr;
        }
        
        if (this->document != nullptr) {
            delete this->document;
            this->document = nullptr;
        }
    }

	void PiPiOperator::init(char* pdfBytes, size_t pdfSize) {
		PoDoFo::PdfMemDocument * document = new PoDoFo::PdfMemDocument();
		auto input = make_shared<PoDoFo::SpanStreamDevice>(pdfBytes, pdfSize);
		document->LoadFromDevice(input);
		
		this->document = document;
        
        PiPiAnnotationObserver* annotObserver = new PiPiAnnotationObserver();
        this->annotObserver = annotObserver;
        
        PiPiFieldObserver* fieldObserver = new PiPiFieldObserver();
        this->fieldObserver = fieldObserver;

		this->pager = nullptr;
		this->editor = nullptr;
		this->filler = nullptr;
		this->extractor = nullptr;
		this->fontManager = nullptr;
        this->fieldObserver = nullptr;
        this->annotObserver = nullptr;
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
        PiPiFontManager* fontManager = getFontManager();
		PiPiEditor* editor = new PiPiEditor(document, fontManager);

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
        
        
        PiPiFontManager* fontManager = this->getFontManager();
        PdfMemDocument* document = this->document;
		
        fontManager->embedFonts();
        document->Save(outputStreamDevice);

		*newPdfSize = outputVector.size();
		*newPdfBytes = new char[*newPdfSize];
		for (size_t i = 0; i < *newPdfSize; i++) {
			*(*newPdfBytes + i) = outputVector[i];
		}
	}
}
