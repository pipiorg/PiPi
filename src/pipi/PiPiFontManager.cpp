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

		PdfFontManager* fontManager = document->GetFonts();
		const PdfFont* font = fontManager->GetOrCreateFontFromBuffer(bufferview(fontBytes, fontSize));
		const std::string fontName = font->GetName();

		std::map<const std::string, const PdfFont>* fontMap = this->fontMap;
		fontMap->insert(fontName, font);
	}

	const PdfFont* PiPiFontManager::accessFont(std::string fontName) {
		std::map<const std::string, const PdfFont>* fontMap = this->fontMap;
		
		const PdfFont* font = fontMap->find(fontName);

		if (font == fontMap->end()) {
			return nullptr;
		}

		return font;
	}
}