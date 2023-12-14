#include "PiPiEditor.h"

namespace PiPi {
	PiPiEditor::PiPiEditor(PdfMemDocument* document, PiPiFontManager* fontManager) {
		this->init(document, fontManager);
	}

	void PiPiEditor::init(PdfMemDocument* document, PiPiFontManager* fontManager) {
		this->document = document;
        this->fontManager = fontManager;
	}

	bool PiPiEditor::isOperable() {
		return this->document != nullptr;
	}

    PiPiEditor* PiPiEditor::flatten() {
        PdfMemDocument* document = this->document;
        
        std::map<const std::string, std::vector<PdfAnnotation*>*>* annotMap = PiPiUtil::SerachAllFieldAnnotation(document);
        
        for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
            std::pair<const std::string, std::vector<PdfAnnotation*>*> pair = *mapIterator;
            
            std::vector<PdfAnnotation*>* annots = pair.second;
            for (auto annotIterator = annots->begin(); annotIterator != annots->end(); annotIterator.operator++()) {
                PdfAnnotation* annotation = *annotIterator;

                PiPiUtil::FlattenAnnotation(annotation);
            }
        }

        delete annotMap;
        
        PiPiUtil::RemoveAllField(document);

        return this;
    }

	PiPiEditor* PiPiEditor::flatten(std::string fieldName) {
		PdfMemDocument* document = this->document;

		std::vector<PdfAnnotation*>* annotations = PiPiUtil::SearchFieldAnnotation(document, fieldName);
		
		for (auto iterator = annotations->begin(); iterator != annotations->end(); ++iterator) {
			const PdfAnnotation* constAnnotation = *iterator;
			PdfAnnotation* annotation = const_cast<PdfAnnotation*>(constAnnotation);

            PiPiUtil::FlattenAnnotation(annotation);
		}

		delete annotations;

		PiPiUtil::RemoveField(document, fieldName);

		return this;
	}

    PiPiEditor* PiPiEditor::addField(std::string fieldName, PiPiFieldType type, unsigned int page, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
		PiPiUtil::CreateField(document, fieldName, type, page, x, y, width, height);
        return this;
    }

	PiPiEditor* PiPiEditor::removeField(std::string fieldName) {
		PdfMemDocument* document = this->document;
		PiPiUtil::RemoveField(document, fieldName);
		return this;
	}

	PiPiEditor* PiPiEditor::renameField(std::string oldFieldName, std::string newFieldName) {
		PdfMemDocument* document = this->document;
		return this;
	}

    PiPiEditor* PiPiEditor::setFieldColor(std::string fieldName, int red, int green, int blue) {
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldBorderColor(std::string fieldName, int red, int green, int blue) {
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldBackgroundColor(std::string fieldName, int red, int green, int blue) {
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment) {
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldMultiline(std::string fieldName, bool multiline) {
        PdfMemDocument* document = this->document;
        
        std::vector<PdfField*>* fields = PiPiUtil::SearchField(document, fieldName);
        
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            
            PdfFieldType type = field->GetType();
            if (type != PdfFieldType::TextBox) {
                continue;
            }
            
            PdfTextBox* textbox = (PdfTextBox*) field;

            textbox->SetMultiLine(multiline);
        }
        
        delete fields;
        
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldFontName(std::string fieldName, std::string fontName) {
        PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
        
        const PdfFont* font = fontManager->accessFont(fontName);
        if (font == nullptr) {
            return;
        }
        
        std::vector<PdfField*>* fields = PiPiUtil::SearchField(document, fieldName);
        
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            
            PdfDictionary& dictRef = field->GetDictionary();
            PdfDictionary* dict = &dictRef;
            
            PdfObject* daObj = dict->FindKey(PdfName("DA"));
            if (daObj == nullptr) {
                continue;
            }
            
            PdfString da = daObj->GetString();
            std::string daString = da.GetString();
            
            std::vector<std::string>* daStringSplits = PiPiCommon::split(daString, " ");
            std::vector<std::string>* newDaStringSplits = new std::vector<std::string>();
            
            for (int i = 0; i < daStringSplits->size(); i++) {
                if (i == 0) {
                    newDaStringSplits->push_back("/" + fontName);
                    continue;
                }
                
                std::string daStringSplit = (*daStringSplits)[i];
                newDaStringSplits->push_back(daStringSplit);
            }
            
            std::string newDaString = PiPiCommon::join(newDaStringSplits, " ");
            
            dict->RemoveKey(PdfName("DA"));
            dict->AddKey(PdfName("DA"), PdfString(newDaString));
            
            delete newDaStringSplits;
            delete daStringSplits;
        }
        
        delete fields;
        
        std::vector<PdfAnnotation*>* annots = PiPiUtil::SearchFieldAnnotation(document, fieldName);
        
        for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++()) {
            PdfAnnotation* annot = *iterator;
            
            PiPiAppearanceUtil::GenerateAppearance(fontManager, annot);
        }
        
        delete annots;
        
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldFontSize(std::string fieldName, float fontSize) {
        return this;
    }
}
