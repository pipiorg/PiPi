#include "PiPiFontManager.h"

namespace PiPi {
	PiPiFontManager::PiPiFontManager(PdfMemDocument* document) {
		this->Init(document);
		this->LoadAcroformFonts();
	}

	bool PiPiFontManager::IsOperable() {
        spdlog::trace("IsOperable");

        return this->operable;
	}

    void PiPiFontManager::EmbedFonts() {
        spdlog::trace("EmbedFonts");

        this->EmbedDocumentFonts();
        this->EmbedAcroformFonts();
    }

	const std::string PiPiFontManager::RegisterFont(char* fontBytes, size_t fontSize) {
        spdlog::trace("RegisterFont");

        if (!this->IsOperable()) {
            throw PiPiFontManageException(PiPiFontManageException::PiPiFontManageExceptionCode::InOperable);
        }

		PdfMemDocument* document = this->document;

		PdfFontManager* fontManager = &(document->GetFonts());

		const PdfFont* font = &(fontManager->GetOrCreateFontFromBuffer(bufferview(fontBytes, fontSize)));
		
		const std::string fontName = font->GetName();

		std::map<const std::string, const PdfFont*>* fontMap = this->fontMap;
		fontMap->insert(std::pair(fontName, font));
        
        return fontName;
	}

	const PdfFont* PiPiFontManager::AccessFont(const std::string fontName) {
        spdlog::trace("AccessFont");

		std::map<const std::string, const PdfFont*>* fontMap = this->fontMap;
		
		auto fontPair = fontMap->find(fontName);

		if (fontPair == fontMap->end()) {
			return nullptr;
		}

		return fontPair->second;
	}

	const PdfFont* PiPiFontManager::AccessDefaultFont() {
        spdlog::trace("AccessDefaultFont");

		PdfMemDocument* document = this->document;
		PdfFontManager* fontManager = &(document->GetFonts());
		PdfFont* font = &(fontManager->GetStandard14Font(PdfStandard14FontType::Helvetica));
		return font;
	}

    void PiPiFontManager::Init(PdfMemDocument* document) {
        spdlog::trace("Init");

        this->operable = true;
        this->document = document;
        this->fontMap = new std::map<const std::string, const PdfFont*>();
    }

    void PiPiFontManager::LoadAcroformFonts() {
        spdlog::trace("LoadAcroformFonts");

        PdfMemDocument* document = this->document;

        PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());

        PdfAcroForm* acroForm = &(document->GetOrCreateAcroForm());

        PdfDictionary* acroformDict = &(acroForm->GetDictionary());

        PdfObject* drObj = acroformDict->FindKey(PdfName("DR"));
        if (drObj == nullptr) {
            return;
        }

        PdfDictionary* dr = &(drObj->GetDictionary());

        PdfObject* fontsObj = dr->FindKey(PdfName("Font"));
        if (fontsObj == nullptr) {
            return;
        }

        PdfDictionary* fonts = &(fontsObj->GetDictionary());

        for (auto iterator = fonts->begin(); iterator != fonts->end(); iterator.operator++()) {
            auto pair = *iterator;
            
            const PdfName& fontNameRef = pair.first;
            const std::string sFontName = fontNameRef.GetString();
            
            const PdfObject& fontReferenceObjectRef = pair.second;
            const PdfReference& fontReferenceRef = fontReferenceObjectRef.GetReference();

            PdfObject* fontObject = indirectObjectList->GetObject(fontReferenceRef);

            std::unique_ptr<PdfFont> fontPtr;
            bool created = PdfFont::TryCreateFromObject(*fontObject, fontPtr);
            if (!created) {
                continue;
            }
            
            const PdfFont* font = fontPtr.get();
            
            this->fontMap->insert(std::pair(sFontName, font));
        }
    }

    void PiPiFontManager::EmbedAcroformFonts() {
        spdlog::trace("EmbedAcroformFonts");

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

    void PiPiFontManager::EmbedDocumentFonts() {
        spdlog::trace("EmbedDocumentFonts");

        PdfMemDocument* document = this->document;
        PdfFontManager* fontManager = &(document->GetFonts());
        fontManager->EmbedFonts();
    }
}
