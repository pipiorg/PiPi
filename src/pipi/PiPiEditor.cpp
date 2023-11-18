#include "PiPiEditor.h"

namespace PiPi {
	PiPiEditor::PiPiEditor(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiEditor::init(PdfMemDocument* document) {
		this->document = document;
	}

	bool PiPiEditor::isOperable() {
		return this->document != nullptr;
	}

    PiPiEditor* PiPiEditor::flatten() {
        PdfMemDocument* docuemnt = this->document;
        
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
}
