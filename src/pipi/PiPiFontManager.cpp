#include "PiPiFontManager.h"

namespace PiPi {
	PiPiFontManager::PiPiFontManager(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiFontManager::init(PdfMemDocument* document) {
		this->document = document;
		this->fontMap = new std::map<const std::string, const PdfFont*>();
	}

	void PiPiFontManager::registerFont(char* fontBytes, size_t fontSize) {
		PdfMemDocument* document = this->document;

		PdfFontManager& fontManagerRef = document->GetFonts();
		PdfFontManager* fontManager = &fontManagerRef;

		const PdfFont& fontRef = fontManager->GetOrCreateFontFromBuffer(bufferview(fontBytes, fontSize));
		const PdfFont* font = &fontRef;
		
		const std::string fontName = font->GetName();

		std::map<const std::string, const PdfFont*>* fontMap = this->fontMap;
		fontMap->insert(std::pair(fontName, font));
	}

	const PdfFont* PiPiFontManager::accessFont(const std::string fontName) {
		std::map<const std::string, const PdfFont*>* fontMap = this->fontMap;
		
		auto fontPair = fontMap->find(fontName);

		if (fontPair == fontMap->end()) {
			return nullptr;
		}

		return fontPair->second;
	}

	const PdfFont* PiPiFontManager::accessDefaultFont() {
		PdfMemDocument* document = this->document;

		PdfFontManager& fontManagerRef = document->GetFonts();
		PdfFontManager* fontManager = &fontManagerRef;

		PdfFont& fontRef = fontManager->GetStandard14Font(PdfStandard14FontType::Helvetica);
		PdfFont* font = &fontRef;

		return font;
	}
}