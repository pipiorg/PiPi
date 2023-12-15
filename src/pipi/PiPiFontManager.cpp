#include "PiPiFontManager.h"

namespace PiPi {
	PiPiFontManager::PiPiFontManager(PdfMemDocument* document) {
		this->init(document);
		this->loadAcroformFonts();
	}

	void PiPiFontManager::init(PdfMemDocument* document) {
		this->document = document;
		this->fontMap = new std::map<const std::string, const PdfFont*>();
	}

	void PiPiFontManager::loadAcroformFonts() {
		PdfMemDocument* document = this->document;

		PdfIndirectObjectList& indirectObjectListRef = document->GetObjects();
		PdfIndirectObjectList* indirectObjectList = &indirectObjectListRef;

		PdfAcroForm* acroForm = document->GetAcroForm();
		if (acroForm == nullptr) {
			return;
		}

		PdfDictionary& acroformDictRef = acroForm->GetDictionary();
		PdfDictionary* acroformDict = &acroformDictRef;

		PdfObject* drObj = acroformDict->FindKey(PdfName("DR"));
		if (drObj == nullptr) {
			return;
		}

		PdfDictionary& drRef = drObj->GetDictionary();
		PdfDictionary* dr = &drRef;

		PdfObject* fontsObj = dr->FindKey(PdfName("Font"));
		if (fontsObj == nullptr) {
			return;
		}

		PdfDictionary& fontsRef = fontsObj->GetDictionary();
		PdfDictionary* fonts = &fontsRef;

		for (auto iterator = fonts->begin(); iterator != fonts->end(); iterator.operator++()) {
			auto pair = *iterator;
			
			const PdfName& fontNameRef = pair.first;
            const std::string sFontName = fontNameRef.GetString();
            
			const PdfObject& fontReferenceObjectRef = pair.second;
			const PdfReference& fontReferenceRef = fontReferenceObjectRef.GetReference();

			PdfObject* fontObject = indirectObjectList->GetObject(fontReferenceRef);
			PdfObject& fontObjectRef = *fontObject;

			std::unique_ptr<PdfFont> fontPtr;
			bool created = PdfFont::TryCreateFromObject(fontObjectRef, fontPtr);
			if (!created) {
				continue;
			}
			
			const PdfFont* font = fontPtr.get();
			
			this->fontMap->insert(std::pair(sFontName, font));
		}
	}

    void PiPiFontManager::embedAcroformFonts() {
        PdfMemDocument* document = this->document;
        
        PdfAcroForm* acroForm = document->GetAcroForm();
        if (acroForm == nullptr) {
            return;
        }

        PdfDictionary& acroformDictRef = acroForm->GetDictionary();
        PdfDictionary* acroformDict = &acroformDictRef;

        PdfObject* drObj = acroformDict->FindKey(PdfName("DR"));
        if (drObj == nullptr) {
            return;
        }

        PdfDictionary& drRef = drObj->GetDictionary();
        PdfDictionary* dr = &drRef;

        PdfObject* fontsObj = dr->FindKey(PdfName("Font"));
        if (fontsObj == nullptr) {
            return;
        }

        PdfDictionary& fontsRef = fontsObj->GetDictionary();
        PdfDictionary* fonts = &fontsRef;
        
        std::map<const std::string, const PdfFont*>* fontMap = this->fontMap;
        
        for (auto iterator = fontMap->begin(); iterator != fontMap->end(); iterator.operator++()) {
            std::pair<const std::string, const PdfFont*> fontPair = *iterator;
            
            const std::string fontName = fontPair.first;
            const PdfFont* font = fontPair.second;
            
            if (fonts->HasKey(PdfName(fontName))) {
                continue;
            }
                
            fonts->AddKeyIndirect(PdfName(fontName), font->GetObject());
        }
    }

	bool PiPiFontManager::isOperable() {
		return this->document != nullptr;
	}

    void PiPiFontManager::embedFonts() {
        PdfMemDocument* document = this->document;
        
        PdfFontManager& fontManagerRef = document->GetFonts();
        PdfFontManager* fontManager = &fontManagerRef;
        
        fontManager->EmbedFonts();
        
        this->embedAcroformFonts();
    }

	const std::string PiPiFontManager::registerFont(char* fontBytes, size_t fontSize) {
		PdfMemDocument* document = this->document;

		PdfFontManager& fontManagerRef = document->GetFonts();
		PdfFontManager* fontManager = &fontManagerRef;

		const PdfFont& fontRef = fontManager->GetOrCreateFontFromBuffer(bufferview(fontBytes, fontSize));
		const PdfFont* font = &fontRef;
		
		const std::string fontName = font->GetName();

		std::map<const std::string, const PdfFont*>* fontMap = this->fontMap;
		fontMap->insert(std::pair(fontName, font));
        
        return fontName;
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