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

    PiPiEditor* PiPiEditor::addFormField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height) {
        PdfMemDocument* document = this->document;
    
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
    
        PdfPage& page = pages->GetPageAt(pageIndex);
    
        Rect pageRect = page.GetRect();
        double pageTop = pageRect.GetTop();
        double pageBottom = pageRect.GetBottom();
        double pageHeight = pageTop - pageBottom;
    
        Rect fieldRect;
        fieldRect.X = x;
        fieldRect.Y = pageHeight - y - height;
        fieldRect.Width = width;
        fieldRect.Height = height;
    
        switch (type) {
            case PiPiFieldType::CheckBox:
                page.CreateField(fieldName, PdfFieldType::CheckBox, fieldRect);
                return this;
            case PiPiFieldType::TextBox:
                page.CreateField(fieldName, PdfFieldType::TextBox, fieldRect);
                return this;
            default:
                return this;
        }
    }

	PiPiEditor* PiPiEditor::removeFormField(std::string fieldName) {
		PdfMemDocument* document = this->document;
		PiPiUtil::RemoveField(document, fieldName);
		return this;
	}

	PiPiEditor* PiPiEditor::renameFormField(std::string oldFieldName, std::string newFieldName) {
		PdfMemDocument* document = this->document;
		return this;
	}
}
