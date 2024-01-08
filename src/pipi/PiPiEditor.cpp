#include "PiPiEditor.h"

namespace PiPi {
	PiPiEditor::PiPiEditor(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAppearanceManager* appearanceManager, PiPiFieldManager* fieldManager) {
        this->operable = true;
        this->document = document;
        this->fontManager = fontManager;
        this->appearanceManager = appearanceManager;
        this->fieldManager = fieldManager;
	}

	bool PiPiEditor::IsOperable() {
        return this->operable;
	}

    PiPiEditor* PiPiEditor::Flatten() {
        spdlog::trace("Flatten");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

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
        spdlog::trace("Flatten");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

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
        spdlog::trace("AddField");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        fieldManager->CreateField(fieldName, type, pageIndex, x, y, width, height);
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

	PiPiEditor* PiPiEditor::RemoveField(std::string fieldName) {
        spdlog::trace("RemoveField");

        return this->RemoveField(fieldName, -1);
	}

    PiPiEditor* PiPiEditor::RemoveField(std::string fieldName, long pageIndex) {
        spdlog::trace("RemoveField");

        return this->RemoveField(fieldName, pageIndex, -1, -1);
    }

    PiPiEditor* PiPiEditor::RemoveField(std::string fieldName, long pageIndex, double x, double y) {
        spdlog::trace("RemoveField");

        return this->RemoveField(fieldName, pageIndex, x, y, -1, -1);
    }

    PiPiEditor* PiPiEditor::RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height) {
        spdlog::trace("RemoveField");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

        PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        fieldManager->RemoveField(fieldName, pageIndex, x, y, width, height);
        appearanceManager->UnMarkNeedAppearance(fieldName);

        return this;
    }

	PiPiEditor* PiPiEditor::RenameField(std::string oldFieldName, std::string newFieldName) {
        spdlog::trace("RenameField");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

		PdfMemDocument* document = this->document;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        fieldManager->RenameField(oldFieldName, newFieldName);

        appearanceManager->UnMarkNeedAppearance(oldFieldName);
        appearanceManager->MarkNeedAppearance(newFieldName);

		return this;
	}

    PiPiEditor* PiPiEditor::SetFieldColor(std::string fieldName, float red, float green, float blue) {
        spdlog::trace("SetFieldColor");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        if (red < 0 || red > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        if (green < 0 || green > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        if (blue < 0 || blue > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            PdfAnnotation* annot = fieldManager->BridgeFieldToAnnotation(field);
            annot->SetColor(PdfColor(red, green, blue));
        }
        
        delete fields;
        
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::SetFieldBorderColor(std::string fieldName, float red, float green, float blue) {
        spdlog::trace("SetFieldBorderColor");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        if (red < 0 || red > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        if (green < 0 || green > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        if (blue < 0 || blue > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            PdfDictionary* fieldDict = &(field->GetDictionary());
            
            PdfObject* fieldMkObj = fieldDict->FindKey(PdfName("MK"));
            if (fieldMkObj == nullptr) {
                fieldDict->AddKey(PdfName("MK"), PdfDictionary());
                fieldMkObj = fieldDict->FindKey(PdfName("MK"));
            }
            
            PdfDictionary* fieldMk = &(fieldMkObj->GetDictionary());
            
            PdfObject* fieldBcObj = fieldMk->FindKey(PdfName("BC"));
            if (fieldBcObj != nullptr) {
                fieldMk->RemoveKey(PdfName("BC"));
            }
            
            PdfArray* colors;
            PiPiColorConverter::ConvertRGBToPoDoFoArray(red, green, blue, &colors);
            
            fieldMk->AddKey(PdfName("BC"), *colors);
        }
        
        delete fields;
        
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::SetFieldBackgroundColor(std::string fieldName, float red, float green, float blue) {
        spdlog::trace("SetFieldBackgroundColor");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        if (red < 0 || red > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        if (green < 0 || green > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        if (blue < 0 || blue > 1) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InvalidColor);
        }
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            PdfDictionary* fieldDict = &(field->GetDictionary());
            
            PdfObject* fieldMkObj = fieldDict->FindKey(PdfName("MK"));
            if (fieldMkObj == nullptr) {
                fieldDict->AddKey(PdfName("MK"), PdfDictionary());
                fieldMkObj = fieldDict->FindKey(PdfName("MK"));
            }
            
            PdfDictionary* fieldMk = &(fieldMkObj->GetDictionary());
            
            PdfObject* fieldBgObj = fieldMk->FindKey(PdfName("BG"));
            if (fieldBgObj != nullptr) {
                fieldMk->RemoveKey(PdfName("BG"));
            }
            
            PdfArray* colors;
            PiPiColorConverter::ConvertRGBToPoDoFoArray(red, green, blue, &colors);
            
            fieldMk->AddKey(PdfName("BG"), *colors);
        }
        
        delete fields;
        
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::SetFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment) {
        spdlog::trace("SetFieldTextHorizontalAlignment");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
        
        std::set<PdfField*>* fields = fieldManager->SearchField(fieldName);
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            PdfDictionary* fieldDict = &(field->GetDictionary());
            
            PdfObject* fieldQObj = fieldDict->FindKey(PdfName("Q"));
            if (fieldQObj != nullptr) {
                fieldDict->RemoveKey(PdfName("Q"));
            }
            
            switch (alignment) {
                case PiPiTextHorizontalAlignment::Center:
                    break;
                    fieldDict->AddKey(PdfName("Q"), PdfObject((int64_t)0));
                case PiPiTextHorizontalAlignment::Right:
                    break;
                    fieldDict->AddKey(PdfName("Q"), PdfObject((int64_t)1));
                case PiPiTextHorizontalAlignment::Left:
                default:
                    fieldDict->AddKey(PdfName("Q"), PdfObject((int64_t)2));
                    break;
            }
        }
        
        delete fields;
        
        appearanceManager->MarkNeedAppearance(fieldName);
        
        return this;
    }

    PiPiEditor* PiPiEditor::SetFieldMultiline(std::string fieldName, bool multiline) {
        spdlog::trace("SetFieldMultiline");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

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
        spdlog::trace("SetFieldFontName");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

        PdfMemDocument* document = this->document;
        PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        
        const PdfFont* font = fontManager->AccessFont(fontName);
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
            
            std::vector<std::string>* daStringSplits = PiPiStringCommon::split(daString, " ");
            std::vector<std::string>* newDaStringSplits = new std::vector<std::string>();
            
            for (int i = 0; i < daStringSplits->size(); i++) {
                if (i == 0) {
                    newDaStringSplits->push_back("/" + fontName);
                    continue;
                }
                
                std::string daStringSplit = (*daStringSplits)[i];
                newDaStringSplits->push_back(daStringSplit);
            }
            
            std::string newDaString = PiPiStringCommon::join(newDaStringSplits, " ");
            
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
        spdlog::trace("SetFieldFontSize");

        if (!this->IsOperable()) {
            throw PiPiEditFieldException(PiPiEditFieldException::PiPiEditFieldExceptionCode::InOperable);
        }

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
            
            std::vector<std::string>* daStringSplits = PiPiStringCommon::split(daString, " ");
            std::vector<std::string>* newDaStringSplits = new std::vector<std::string>();
            
            for (int i = 0; i < daStringSplits->size(); i++) {
                if (i == 1) {
                    newDaStringSplits->push_back(std::to_string(fontSize));
                    continue;
                }
                
                std::string daStringSplit = (*daStringSplits)[i];
                newDaStringSplits->push_back(daStringSplit);
            }
            
            std::string newDaString = PiPiStringCommon::join(newDaStringSplits, " ");
            
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
        spdlog::trace("GetDefaultDA");

        PdfDocument* document = this->document;
        
        PdfAcroForm* acroform = document->GetAcroForm();
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformDAObj = acroformDict->FindKey(PdfName("DA"));
        const PdfString* acroformDA = &(acroformDAObj->GetString());
        
        return acroformDA;
    }
}
