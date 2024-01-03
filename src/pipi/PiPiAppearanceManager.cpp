#include "PiPiAppearanceManager.h"

namespace PiPi {
	PiPiAppearanceManager::PiPiAppearanceManager(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiFieldManager* fieldManager) {
		this->document = document;
		this->fontManager = fontManager;
        this->fieldManager = fieldManager;

		this->fieldNames = new std::set<std::string>();
	}

	PiPiAppearanceManager::~PiPiAppearanceManager() {
		if (this->fieldNames) {
			delete this->fieldNames;
			this->fieldNames = nullptr;
		}
	}

	bool PiPiAppearanceManager::IsOperable() {
		return this->document != nullptr;
	}

	void PiPiAppearanceManager::GenerateAppearance() {
		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = this->fontManager;
        PiPiFieldManager* fieldManager = this->fieldManager;
		std::set<std::string>* fieldNames = this->fieldNames;
        
        std::map<const std::string, std::set<PdfField*>*>* fieldMap = fieldManager->SearchAllField();
        for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++()) {
            std::set<PdfField*>* fields = mapIterator->second;
            
            for (auto fieldIterator = fields->begin(); fieldIterator != fields->end(); fieldIterator.operator++()) {
                PdfField* field = *fieldIterator;
                PdfAnnotation* annot = fieldManager->BridgeFieldToAnnotation(field);
                
                PdfFieldType type = field->GetType();
                std::string fieldName = field->GetFullName();
                
                if (type == PdfFieldType::CheckBox || fieldNames->find(fieldName) != fieldNames->end()) {
                    PiPiAppearanceUtil::GenerateAppearance(fontManager, fieldManager, annot);
                }
            }
        }
	}

	void PiPiAppearanceManager::MarkNeedAppearance(std::string fieldName) {
		std::set<std::string>* fieldNames = this->fieldNames;
		fieldNames->insert(fieldName);
	}

	void PiPiAppearanceManager::UnMarkNeedAppearance(std::string fieldName) {
		std::set<std::string>* fieldNames = this->fieldNames;
		fieldNames->erase(fieldName);
	}

	void PiPiAppearanceManager::ClearNeedAppearance() {
		std::set<std::string>* fieldNames = this->fieldNames;
		fieldNames->clear();
	}
}
