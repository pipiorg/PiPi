#include "PiPiEditor.h"

namespace PiPi {
	PiPiEditor::PiPiEditor(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiEditor::init(PdfMemDocument* document) {
		this->document = document;
	}

	PiPiEditor* PiPiEditor::flatten(std::string fieldName) {
		PoDoFo::PdfMemDocument* document = this->document;

		return this;
	}

	PiPiEditor* PiPiEditor::removeFormField(std::string fieldName) {
		PdfMemDocument* document = this->document;

		// Delete From Acroform
		PdfAcroForm* acroform = document->GetAcroForm();
		unsigned int fieldCount = acroform->GetFieldCount();
		unsigned int fieldIndex = 0;
		std::vector<unsigned int>* removeFieldIndexs = new std::vector<unsigned int>();
		for (auto iterator = acroform->begin(); iterator.operator!=(acroform->end()); iterator.operator++()) {
			PdfField* field = iterator.operator*();
			nullable<const PdfString &> name = field->GetName();
			if (name.has_value()) {
				std::string nameString = name.value().GetString();
				if (nameString == fieldName) {
					removeFieldIndexs->push_back(fieldIndex);
				}
			}
			fieldIndex++;
		}

		std::sort(removeFieldIndexs->begin(), removeFieldIndexs->end(), std::greater<unsigned int>());
		for (auto iterator = removeFieldIndexs->begin(); iterator.operator!=(removeFieldIndexs->end()); iterator.operator++()) {
			unsigned int removeFieldIndex = iterator.operator*();
			acroform->RemoveFieldAt(removeFieldIndex);
		}

		// Delete From Page
		PdfPageCollection& pages = document->GetPages();
		unsigned int pageCount = pages.GetCount();
		for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
			PdfPage& page = pages.GetPageAt(pageIndex);

			std::vector<unsigned int>* removeAnnotIndexs = new std::vector<unsigned int>();
			PdfAnnotationCollection& annotations = page.GetAnnotations();
			unsigned int annotCount = annotations.GetCount();
			for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++) {
				PdfAnnotation& annotation = annotations.GetAnnotAt(annotIndex);
				PdfAnnotationType annotType = annotation.GetType();
				nullable<const PdfString&> annotTitle = annotation.GetTitle();

				if (annotType != PdfAnnotationType::Widget) {
					continue;
				}

				if (!annotTitle.has_value()) {
					continue;
				}

				std::string annotTitleString = annotTitle->GetString();
				if (annotTitleString == fieldName) {
					removeAnnotIndexs->push_back(annotIndex);
				}
			}

			std::sort(removeAnnotIndexs->begin(), removeAnnotIndexs->end(), std::greater<unsigned int>());
			for (auto iterator = removeAnnotIndexs->begin(); iterator.operator!=(removeAnnotIndexs->end()); iterator.operator++()) {
				unsigned int removeAnnotIndex = iterator.operator*();
				annotations.RemoveAnnotAt(removeAnnotIndex);
			}
		}

		document->CollectGarbage();

		return this;
	}

	PiPiEditor* PiPiEditor::renameFormField(std::string oldFieldName, std::string newFieldName) {
		return this;
	}

	
}