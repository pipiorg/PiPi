#include "PiPiEditor.h"

namespace PiPi {
	PiPiEditor::PiPiEditor(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAppearanceManager* appearanceManager, PiPiFieldManager* fieldManager) {
        this->document = document;
        this->fontManager = fontManager;
        this->appearanceManager = appearanceManager;
        this->fieldManager = fieldManager;
	}

	bool PiPiEditor::IsOperable() {
		return this->document != nullptr;
	}

    PiPiEditor* PiPiEditor::Flatten() {
        PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = fieldManager->SearchAllField();
        
        for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++()) {
            std::set<PdfField*>* fields = mapIterator->second;
            
            for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
                PdfField* field = *iterator;
                PdfAnnotation* annot = fieldManager->BridgeFieldToAnnotation(field);
                
                PiPiAppearanceUtil::FlattenAppearance(fontManager, fieldManager, annot);
            }
        }
        
        delete fieldMap;
        
        fieldManager->RemoveAllField();
        appearanceManager->ClearNeedAppearance();

        return this;
    }

	PiPiEditor* PiPiEditor::Flatten(std::string fieldName) {
		PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
		
		for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            PdfAnnotation* annot = fieldManager->BridgeFieldToAnnotation(field);
            
            PiPiAppearanceUtil::FlattenAppearance(fontManager, fieldManager, annot);
		}

		delete fields;

        fieldManager->RemoveField(fieldName);
        appearanceManager->UnMarkNeedAppearance(fieldName);

		return this;
	}

    PiPiEditor* PiPiEditor::AddField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        fieldManager->CreateField(fieldName, type, pageIndex, x, y, width, height);
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

	PiPiEditor* PiPiEditor::RemoveField(std::string fieldName) {
        return this->RemoveField(fieldName, -1);
	}

    PiPiEditor* PiPiEditor::RemoveField(std::string fieldName, long pageIndex) {
        return this->RemoveField(fieldName, pageIndex, -1, -1);
    }

    PiPiEditor* PiPiEditor::RemoveField(std::string fieldName, long pageIndex, double x, double y) {
        return this->RemoveField(fieldName, pageIndex, x, y, -1, -1);
    }

    PiPiEditor* PiPiEditor::RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        fieldManager->RemoveField(fieldName, pageIndex, x, y, width, height);
        appearanceManager->UnMarkNeedAppearance(fieldName);

        return this;
    }

	PiPiEditor* PiPiEditor::RenameField(std::string oldFieldName, std::string newFieldName) {
		PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        fieldManager->RenameField(oldFieldName, newFieldName);

        appearanceManager->UnMarkNeedAppearance(oldFieldName);
        appearanceManager->MarkNeedAppearance(newFieldName);

		return this;
	}

    PiPiEditor* PiPiEditor::SetFieldColor(std::string fieldName, int red, int green, int blue) {
        throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::NotImplementate);
    }

    PiPiEditor* PiPiEditor::SetFieldBorderColor(std::string fieldName, int red, int green, int blue) {
        throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::NotImplementate);
    }

    PiPiEditor* PiPiEditor::SetFieldBackgroundColor(std::string fieldName, int red, int green, int blue) {
        throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::NotImplementate);
    }

    PiPiEditor* PiPiEditor::SetFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment) {
        throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::NotImplementate);
    }

    PiPiEditor* PiPiEditor::SetFieldMultiline(std::string fieldName, bool multiline) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
        
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            
            PdfFieldType type = field->GetType();
            if (type != PdfFieldType::TextBox) {
                throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::MultilineNotSupported);
            }
            
            PdfTextBox* textbox = (PdfTextBox*) field;
            textbox->SetMultiLine(multiline);
        }
        
        delete fields;

        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::SetFieldFontName(std::string fieldName, std::string fontName) {
        PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        
        const PdfFont* font = fontManager->accessFont(fontName);
        if (font == nullptr) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::NotRegisterFont);
        }
        
        const PdfString* defaultDA = this->GetDefaultDA();
        std::string defaultDAString = defaultDA->GetString();
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            
            PdfDictionary* dict = &(field->GetDictionary());
            
            PdfDictionary* parentDict = dict->FindKeyAs<PdfDictionary*>(PdfName("Parent"));
            PdfObject* daObj = dict->FindKey(PdfName("DA"));
            while (daObj == nullptr && parentDict != nullptr) {
                daObj = parentDict->FindKey(PdfName("DA"));
                parentDict = parentDict->FindKeyAs<PdfDictionary*>(PdfName("Parent"));
            }
            
            std::string daString = daObj == nullptr
                ? defaultDAString
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

    PiPiEditor* PiPiEditor::SetFieldFontSize(std::string fieldName, float fontSize) {
        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        const PdfString* defaultDA = this->GetDefaultDA();
        std::string defaultDAString = defaultDA->GetString();
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
        
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            
            PdfDictionary* dict = &(field->GetDictionary());
            
            PdfDictionary* parentDict = dict->FindKeyAs<PdfDictionary*>(PdfName("Parent"));
            PdfObject* daObj = dict->FindKey(PdfName("DA"));
            while (daObj == nullptr && parentDict != nullptr) {
                daObj = parentDict->FindKey(PdfName("DA"));
                parentDict = parentDict->FindKeyAs<PdfDictionary*>(PdfName("Parent"));
            }
            
            std::string daString = daObj == nullptr
                ? defaultDAString
                : daObj->GetString().GetString();
            
            std::vector<std::string>* daStringSplits = PiPiCommon::split(daString, " ");
            std::vector<std::string>* newDaStringSplits = new std::vector<std::string>();
            
            for (int i = 0; i < daStringSplits->size(); i++) {
                if (i == 1) {
                    newDaStringSplits->push_back(std::to_string(fontSize));
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

    const PdfString* PiPiEditor::GetDefaultDA() {
        PdfDocument* document = this->document;
        
        PdfAcroForm* acroform = document->GetAcroForm();
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformDAObj = acroformDict->FindKey(PdfName("DA"));
        const PdfString* acroformDA = &(acroformDAObj->GetString());
        
        return acroformDA;
    }
}
