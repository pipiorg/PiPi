#include "PiPiAppearanceManager.h"

namespace PiPi {
	PiPiAppearanceManager::PiPiAppearanceManager(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAnnotationObserver* annotObserver) {
		this->document = document;
		this->fontManager = fontManager;
		this->annotObserver = annotObserver;

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
		PiPiAnnotationObserver* annotObserver = this->annotObserver;
		PiPiFontManager* fontManager = this->fontManager;
		std::set<std::string>* fieldNames = this->fieldNames;

		std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = PiPiFieldUtil::SerachAllFieldAnnotation(annotObserver, document);

		for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++()) {
			std::set<PdfAnnotation*>* annots = mapIterator->second;

			for (auto annotIterator = annots->begin(); annotIterator != annots->end(); annotIterator.operator++()) {
				PdfAnnotation* annot = *annotIterator;

				PdfObject& objRef = annot->GetObject();
				PdfObject* obj = &objRef;

				std::unique_ptr<PdfField> fieldPtr;
				bool created = PdfField::TryCreateFromObject(objRef, fieldPtr);
				if (!created) {
					continue;
				}

				PdfField* field = fieldPtr.get();

				PdfFieldType type = field->GetType();
				if (type == PdfFieldType::CheckBox) {
					PiPiAppearanceUtil::GenerateAppearance(fontManager, annot);
					continue;
				}

				std::string fieldName = field->GetFullName();
				if (fieldNames->find(fieldName) != fieldNames->end()) {
					PiPiAppearanceUtil::GenerateAppearance(fontManager, annot);
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
