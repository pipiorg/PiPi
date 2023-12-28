#include "PiPiOperator.h"

namespace PiPi {
	PiPiOperator::PiPiOperator(char* pdfBytes, size_t pdfSize) {
		PoDoFo::PdfMemDocument* document = new PoDoFo::PdfMemDocument();
		auto input = make_shared<PoDoFo::SpanStreamDevice>(pdfBytes, pdfSize);
		document->LoadFromDevice(input);

		this->document = document;

		PiPiFontManager* fontManager = new PiPiFontManager(document);
		this->fontManager = fontManager;
        
        PiPiFieldManager* fieldManager = new PiPiFieldManager(document);
        this->fieldManager = fieldManager;

		PiPiAppearanceManager* appearanceManager = new PiPiAppearanceManager(document, fontManager, annotObserver);
		this->appearanceManager = appearanceManager;

		this->pager = nullptr;
		this->editor = nullptr;
		this->filler = nullptr;
		this->extractor = nullptr;
	}

    PiPiOperator::~PiPiOperator() {
		if (this->appearanceManager != nullptr) {
			delete this->appearanceManager;
			this->appearanceManager = nullptr;
		}

        if (this->fieldManager != nullptr) {
            delete this->fieldManager;
            this->fieldManager = nullptr;
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
            this->document->CollectGarbage();
            
            delete this->document;
            this->document = nullptr;
        }
    }

	PiPiFontManager* PiPiOperator::getFontManager() {
		return this->fontManager;
	}

	PiPiFiller* PiPiOperator::getFiller() {
		if (this->filler != nullptr) {
			return this->filler;
		}

		PdfMemDocument* document = this->document;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
		PiPiFontManager* fontManager = this->fontManager;
		PiPiAppearanceManager* appearanceManager = this->appearanceManager;
		
        PiPiFiller* filler = new PiPiFiller(document, fontManager, appearanceManager, fieldObserver, annotObserver);

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
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        PiPiFontManager* fontManager = this->fontManager;
		PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        
		PiPiEditor* editor = new PiPiEditor(document, fontManager, appearanceManager, fieldObserver, annotObserver);

		this->editor = editor;

		return this->editor;
	}

	PiPiExtractor* PiPiOperator::getExtractor() {
		if (this->extractor != nullptr) {
			return this->extractor;
		}

		PdfMemDocument* document = this->document;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        
		PiPiExtractor* extractor = new PiPiExtractor(document, annotObserver);

		this->extractor = extractor;

		return this->extractor;
	}

	bool PiPiOperator::isOperable() {
		return this->document != nullptr;
	}

	void PiPiOperator::finalize(char** newPdfBytes, size_t* newPdfSize) {
		vector<char> outputVector;
		PoDoFo::VectorStreamDevice outputStreamDevice(outputVector);
        
        PiPiFontManager* fontManager = this->getFontManager();
		PiPiAppearanceManager* appearanceManager = this->appearanceManager;
		PdfMemDocument* document = this->document;
		
		appearanceManager->GenerateAppearance();
        fontManager->embedFonts();
        document->Save(outputStreamDevice);

		*newPdfSize = outputVector.size();
		*newPdfBytes = new char[*newPdfSize];
		for (size_t i = 0; i < *newPdfSize; i++) {
			*(*newPdfBytes + i) = outputVector[i];
		}
	}
}
