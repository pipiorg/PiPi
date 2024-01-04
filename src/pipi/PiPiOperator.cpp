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

		PiPiAppearanceManager* appearanceManager = new PiPiAppearanceManager(document, fontManager, fieldManager);
		this->appearanceManager = appearanceManager;

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

	PiPiFontManager* PiPiOperator::GetFontManager() {
		return this->fontManager;
	}

	PiPiFiller* PiPiOperator::GetFiller() {
		if (this->filler != nullptr) {
			return this->filler;
		}

		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = this->fontManager;
		PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
		
        PiPiFiller* filler = new PiPiFiller(document, fontManager, appearanceManager, fieldManager);

		this->filler = filler;

		return this->filler;
	}

	PiPiEditor* PiPiOperator::GetEditor() {
		if (this->editor != nullptr) {
			return this->editor;
		}

		PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
		PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
		PiPiEditor* editor = new PiPiEditor(document, fontManager, appearanceManager, fieldManager);

		this->editor = editor;

		return this->editor;
	}

	PiPiExtractor* PiPiOperator::GetExtractor() {
		if (this->extractor != nullptr) {
			return this->extractor;
		}

		PdfMemDocument* document = this->document;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
		PiPiExtractor* extractor = new PiPiExtractor(document, fieldManager);

		this->extractor = extractor;

		return this->extractor;
	}

	bool PiPiOperator::IsOperable() {
		return this->document != nullptr;
	}

	void PiPiOperator::Finalize(std::vector<char>** newPdf) {
        *newPdf = new std::vector<char>();
		PoDoFo::VectorStreamDevice outputStreamDevice(**newPdf);
        
        PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
		PiPiAppearanceManager* appearanceManager = this->appearanceManager;
		
		appearanceManager->GenerateAppearance();
        fontManager->EmbedFonts();
        document->Save(outputStreamDevice);
	}
}
