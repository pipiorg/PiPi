#include "PiPiFontManager.h"

namespace PiPi
{
	PiPiFontManager::PiPiFontManager(PdfMemDocument *document)
	{
		this->Init(document);
		this->LoadAcroformFonts();
	}

	PiPiFontManager::~PiPiFontManager()
	{
		if (this->fontMap != nullptr)
		{
			for (auto iterator = this->fontMap->begin(); iterator != this->fontMap->end(); iterator.operator++())
			{
				std::string fontName = iterator->first;

				if (this->nonStandardFontNames->find(fontName) != this->nonStandardFontNames->end())
				{
					delete iterator->second;
					iterator->second = nullptr;
				}
			}

			delete this->fontMap;
			this->fontMap = nullptr;
		}

		if (this->nonStandardFontNames != nullptr)
		{
			delete this->nonStandardFontNames;
			this->nonStandardFontNames = nullptr;
		}
	}

	void PiPiFontManager::EmbedFonts()
	{
		spdlog::trace("EmbedFonts");

		this->EmbedDocumentFonts();
		this->EmbedAcroformFonts();
	}

	const std::string PiPiFontManager::RegisterFont(char *fontBytes, size_t fontSize)
	{
		spdlog::trace("RegisterFont");

		PdfMemDocument *document = this->document;

		PdfFontManager *fontManager = &(document->GetFonts());

		const PdfFont *font = &(fontManager->GetOrCreateFontFromBuffer(bufferview(fontBytes, fontSize)));

		const std::string fontName = font->GetName();

		std::map<const std::string, const PdfFont *> *fontMap = this->fontMap;
		fontMap->insert(std::pair(fontName, font));

		return fontName;
	}

	const PdfFont *PiPiFontManager::AccessFont(const std::string fontName)
	{
		spdlog::trace("AccessFont");

		std::map<const std::string, const PdfFont *> *fontMap = this->fontMap;

		auto fontPair = fontMap->find(fontName);

		if (fontPair == fontMap->end())
		{
			return nullptr;
		}

		return fontPair->second;
	}

	const PdfFont *PiPiFontManager::AccessDefaultFont()
	{
		spdlog::trace("AccessDefaultFont");

		PdfMemDocument *document = this->document;
		PdfFontManager *fontManager = &(document->GetFonts());
		PdfFont *font = &(fontManager->GetStandard14Font(PdfStandard14FontType::Helvetica));
		return font;
	}

	void PiPiFontManager::Init(PdfMemDocument *document)
	{
		spdlog::trace("Init");

		this->document = document;
		this->fontMap = new std::map<const std::string, const PdfFont *>();
		this->nonStandardFontNames = new std::set<std::string>();
	}

	void PiPiFontManager::LoadAcroformFonts()
	{
		spdlog::trace("LoadAcroformFonts");

		PdfMemDocument *document = this->document;

		PdfIndirectObjectList *indirectObjectList = &(document->GetObjects());

		PdfAcroForm *acroForm = &(document->GetOrCreateAcroForm());

		PdfDictionary *acroformDict = &(acroForm->GetDictionary());

		PdfObject *drObj = acroformDict->FindKey(PdfName("DR"));
		if (drObj == nullptr)
		{
			return;
		}

		PdfDictionary *dr = &(drObj->GetDictionary());

		PdfObject *fontsObj = dr->FindKey(PdfName("Font"));
		if (fontsObj == nullptr)
		{
			return;
		}

		PdfDictionary *fonts = &(fontsObj->GetDictionary());

		for (auto iterator = fonts->begin(); iterator != fonts->end(); iterator.operator++())
		{
			const PdfName *fontName = &(iterator->first);
			std::string fontNameString = fontName->GetString();

			PdfObject *fontObj = fonts->FindKey(PdfName(fontNameString));
			PdfDictionary *fontDict = &(fontObj->GetDictionary());

			PdfObject *fontBaseFontObj = fontDict->FindKey(PdfName("BaseFont"));
			const PdfName *fontBaseFont = &(fontBaseFontObj->GetName());
			std::string fontBaseFontString = fontBaseFont->GetString();

			PdfStandard14FontType stdFont;
			if (PdfFont::IsStandard14Font(fontBaseFontString, true, stdFont))
			{
				PdfFontManager *fontManager = &(document->GetFonts());

				PdfFontSearchParams searchParams;
				searchParams.AutoSelect = PdfFontAutoSelectBehavior::Standard14Alt;

				PdfFontCreateParams createParams;

				PdfFont *font = fontManager->SearchFont(fontBaseFontString, searchParams, createParams);

				this->fontMap->insert(std::pair(fontNameString, font));

				continue;
			}

			std::unique_ptr<PdfFont> fontPtr;
			bool created = PdfFont::TryCreateFromObject(*fontObj, fontPtr);
			if (!created)
			{
				continue;
			}

			PdfFont *font = fontPtr.release();

			this->nonStandardFontNames->insert(fontNameString);
			this->fontMap->insert(std::pair(fontNameString, font));
		}
	}

	void PiPiFontManager::EmbedAcroformFonts()
	{
		spdlog::trace("EmbedAcroformFonts");

		PdfMemDocument *document = this->document;

		PdfAcroForm *acroForm = document->GetAcroForm();
		if (acroForm == nullptr)
		{
			return;
		}

		PdfDictionary *acroformDict = &(acroForm->GetDictionary());

		PdfObject *drObj = acroformDict->FindKey(PdfName("DR"));
		if (drObj == nullptr)
		{
			return;
		}

		PdfDictionary *dr = &(drObj->GetDictionary());

		PdfObject *fontsObj = dr->FindKey(PdfName("Font"));
		if (fontsObj == nullptr)
		{
			return;
		}

		PdfDictionary *fonts = &(fontsObj->GetDictionary());

		std::map<const std::string, const PdfFont *> *fontMap = this->fontMap;

		for (auto iterator = fontMap->begin(); iterator != fontMap->end(); iterator.operator++())
		{
			std::pair<const std::string, const PdfFont *> fontPair = *iterator;

			const std::string fontName = fontPair.first;
			const PdfFont *font = fontPair.second;

			if (fonts->HasKey(PdfName(fontName)))
			{
				continue;
			}

			fonts->AddKeyIndirect(PdfName(fontName), font->GetObject());
		}
	}

	void PiPiFontManager::EmbedDocumentFonts()
	{
		spdlog::trace("EmbedDocumentFonts");

		PdfMemDocument *document = this->document;
		PdfFontManager *fontManager = &(document->GetFonts());
		fontManager->EmbedFonts();
	}
}
