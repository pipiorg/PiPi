#include "PiPiOperator.h"

namespace PiPi
{
	PiPiOperator::PiPiOperator(char *pdfBytes, size_t pdfSize)
	{
		PoDoFo::PdfMemDocument *document = new PoDoFo::PdfMemDocument();
		auto input = make_shared<PoDoFo::SpanStreamDevice>(pdfBytes, pdfSize);
		document->LoadFromDevice(input);

		this->document = document;

		PiPiFontManager *fontManager = new PiPiFontManager(document);
		this->fontManager = fontManager;

		PiPiFieldManager *fieldManager = new PiPiFieldManager(document);
		this->fieldManager = fieldManager;

		PiPiAppearanceManager *appearanceManager = new PiPiAppearanceManager(document, fontManager, fieldManager);
		this->appearanceManager = appearanceManager;

		this->editor = nullptr;
		this->filler = nullptr;
		this->extractor = nullptr;
	}

	PiPiOperator::~PiPiOperator()
	{
		if (this->appearanceManager != nullptr)
		{
			delete this->appearanceManager;
			this->appearanceManager = nullptr;
		}

		if (this->fieldManager != nullptr)
		{
			delete this->fieldManager;
			this->fieldManager = nullptr;
		}

		if (this->document != nullptr)
		{
			this->document->CollectGarbage();

			delete this->document;
			this->document = nullptr;
		}

		if (this->fontManager != nullptr)
		{
			this->fontManager->operable = false;
		}

		if (this->editor != nullptr)
		{
			this->editor->operable = false;
		}

		if (this->filler != nullptr)
		{
			this->filler->operable = false;
		}

		if (this->extractor != nullptr)
		{
			this->extractor->operable = false;
		}
	}

	PiPiFontManager *PiPiOperator::GetFontManager()
	{
		spdlog::trace("GetFontManager");

		return this->fontManager;
	}

	PiPiFiller *PiPiOperator::GetFiller()
	{
		spdlog::trace("GetFiller");

		if (this->filler != nullptr)
		{
			return this->filler;
		}

		PdfMemDocument *document = this->document;
		PiPiFontManager *fontManager = this->fontManager;
		PiPiAppearanceManager *appearanceManager = this->appearanceManager;
		PiPiFieldManager *fieldManager = this->fieldManager;

		PiPiFiller *filler = new PiPiFiller(document, fontManager, appearanceManager, fieldManager);

		this->filler = filler;

		return this->filler;
	}

	PiPiEditor *PiPiOperator::GetEditor()
	{
		spdlog::trace("GetEditor");

		if (this->editor != nullptr)
		{
			return this->editor;
		}

		PdfMemDocument *document = this->document;
		PiPiFontManager *fontManager = this->fontManager;
		PiPiAppearanceManager *appearanceManager = this->appearanceManager;
		PiPiFieldManager *fieldManager = this->fieldManager;

		PiPiEditor *editor = new PiPiEditor(document, fontManager, appearanceManager, fieldManager);

		this->editor = editor;

		return this->editor;
	}

	PiPiExtractor *PiPiOperator::GetExtractor()
	{
		spdlog::trace("GetExtractor");

		if (this->extractor != nullptr)
		{
			return this->extractor;
		}

		PdfMemDocument *document = this->document;
		PiPiFieldManager *fieldManager = this->fieldManager;

		PiPiExtractor *extractor = new PiPiExtractor(document, fieldManager);

		this->extractor = extractor;

		return this->extractor;
	}

	bool PiPiOperator::IsOperable()
	{
		spdlog::trace("IsOperable");

		return this->document != nullptr;
	}

	void PiPiOperator::Finalize(std::vector<char> **newPdf)
	{
		spdlog::trace("Finalize");

		*newPdf = new std::vector<char>();
		PoDoFo::VectorStreamDevice outputStreamDevice(**newPdf);

		PdfMemDocument *document = this->document;
		PiPiFontManager *fontManager = this->fontManager;
		PiPiAppearanceManager *appearanceManager = this->appearanceManager;

		appearanceManager->GenerateAppearance();
		fontManager->EmbedFonts();
		document->Save(outputStreamDevice);
	}
}
