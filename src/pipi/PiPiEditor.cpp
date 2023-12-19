#include "PiPiEditor.h"

namespace PiPi {
	PiPiEditor::PiPiEditor(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAppearanceManager* appearanceManager, PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver) {
        this->document = document;
        this->fontManager = fontManager;
        this->appearanceManager = appearanceManager;
        this->fieldObserver = fieldObserver;
        this->annotObserver = annotObserver;
	}

	bool PiPiEditor::isOperable() {
		return this->document != nullptr;
	}

    PiPiEditor* PiPiEditor::flatten() {
        PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = PiPiAnnotationUtil::SerachAllFieldAnnotation(annotObserver, document);
        
        for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
            std::pair<const std::string, std::set<PdfAnnotation*>*> pair = *mapIterator;
            
            std::set<PdfAnnotation*>* annots = pair.second;
            for (auto annotIterator = annots->begin(); annotIterator != annots->end(); annotIterator.operator++()) {
                PdfAnnotation* annotation = *annotIterator;

                PiPiAppearanceUtil::GenerateAppearance(fontManager, annotation);
                PiPiAnnotationUtil::FlattenAnnotation(annotation);
            }
        }

        delete annotMap;
        
        PiPiAnnotationUtil::RemoveAllFieldAnnotation(annotObserver, document);
        PiPiFieldUtil::RemoveAllField(fieldObserver, document);

        appearanceManager->ClearNeedAppearance();

        return this;
    }

	PiPiEditor* PiPiEditor::flatten(std::string fieldName) {
		PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;

		std::set<PdfAnnotation*>* annotations = PiPiAnnotationUtil::SearchFieldAnnotation(annotObserver, document, fieldName);
		
		for (auto iterator = annotations->begin(); iterator != annotations->end(); ++iterator) {
			const PdfAnnotation* constAnnotation = *iterator;
			PdfAnnotation* annotation = const_cast<PdfAnnotation*>(constAnnotation);

            PiPiAppearanceUtil::GenerateAppearance(fontManager, annotation);
            PiPiAnnotationUtil::FlattenAnnotation(annotation);
		}

		delete annotations;

		PiPiFieldUtil::RemoveField(fieldObserver, annotObserver, document, fieldName);
        PiPiAnnotationUtil::RemoveFieldAnnotation(annotObserver, document, fieldName);

        appearanceManager->UnMarkNeedAppearance(fieldName);

		return this;
	}

    PiPiEditor* PiPiEditor::addField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        
        PiPiFieldUtil::CreateField(fieldObserver, annotObserver, document, fieldName, type, pageIndex, x, y, width, height);
        
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

	PiPiEditor* PiPiEditor::removeField(std::string fieldName) {
        return this->removeField(fieldName, -1);
	}

    PiPiEditor* PiPiEditor::removeField(std::string fieldName, unsigned int pageIndex) {
        return this->removeField(fieldName, pageIndex, -1, -1);
    }

    PiPiEditor* PiPiEditor::removeField(std::string fieldName, unsigned int pageIndex, double x, double y) {
        return this->removeField(fieldName, pageIndex, x, y, -1, -1);
    }

    PiPiEditor* PiPiEditor::removeField(std::string fieldName, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        
        PiPiFieldUtil::RemoveField(fieldObserver, annotObserver, document, fieldName, pageIndex, x, y, width, height);
        PiPiAnnotationUtil::RemoveFieldAnnotation(annotObserver, document, fieldName, pageIndex, x, y, width, height);
        
        appearanceManager->UnMarkNeedAppearance(fieldName);

        return this;
    }

	PiPiEditor* PiPiEditor::renameField(std::string oldFieldName, std::string newFieldName) {
		PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        
        // TODO: rename field feature

        appearanceManager->UnMarkNeedAppearance(oldFieldName);
        appearanceManager->MarkNeedAppearance(newFieldName);

		return this;
	}

    PiPiEditor* PiPiEditor::setFieldColor(std::string fieldName, int red, int green, int blue) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;

        // TODO: set field color feature
        
        appearanceManager->MarkNeedAppearance(fieldName);

        return this;
    }

    PiPiEditor* PiPiEditor::setFieldBorderColor(std::string fieldName, int red, int green, int blue) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        
        // TODO: set field border color feature

        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldBackgroundColor(std::string fieldName, int red, int green, int blue) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;

        // TODO: set background color feature
        
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        
        // TODO: set field text horizontal alignment feature

        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldMultiline(std::string fieldName, bool multiline) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        
        std::set<PdfField*>* fields = PiPiFieldUtil::SearchField(fieldObserver, document, fieldName);
        
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

        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldFontName(std::string fieldName, std::string fontName) {
        PdfMemDocument* document = this->document;
        PdfAcroForm* acroform = document->GetAcroForm();
        
        PdfDictionary& acroformDictRef = acroform->GetDictionary();
        PdfDictionary* acroformDict = &acroformDictRef;
        
        PdfObject* acroformDAObj = acroformDict->FindKey(PdfName("DA"));
        const PdfString& acroformDARef = acroformDAObj->GetString();
        const PdfString* acroformDA = &acroformDARef;
        
        std::string dDa = acroformDA->GetString();
        
        PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        
        const PdfFont* font = fontManager->accessFont(fontName);
        if (font == nullptr) {
            return this;
        }
        
        std::set<PdfField*>* fields = PiPiFieldUtil::SearchField(fieldObserver, document, fieldName);
        
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            
            PdfDictionary& dictRef = field->GetDictionary();
            PdfDictionary* dict = &dictRef;
            
            PdfDictionary* parentDict = dict->FindKeyAs<PdfDictionary*>(PdfName("Parent"));
            PdfObject* daObj = dict->FindKey(PdfName("DA"));
            while (daObj == nullptr && parentDict != nullptr) {
                daObj = parentDict->FindKey(PdfName("DA"));
                parentDict = parentDict->FindKeyAs<PdfDictionary*>(PdfName("Parent"));
            }
            
            std::string daString = daObj == nullptr
                ? dDa
                : daObj->GetString().GetString();
            
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
        
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::setFieldFontSize(std::string fieldName, float fontSize) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        
        // TODO: set field font size feature

        appearanceManager->MarkNeedAppearance(fieldName);

        return this;
    }
}
