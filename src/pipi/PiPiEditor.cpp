#include "PiPiEditor.h"

namespace PiPi {
	PiPiEditor::PiPiEditor(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiEditor::init(PdfMemDocument* document) {
		this->document = document;
	}

	PiPiEditor* PiPiEditor::flatten(std::string fieldName) {
		PdfMemDocument* document = this->document;
		PdfMemDocument& documentRef = *document;
		PdfAcroForm* acroform = document->GetAcroForm();

		PdfPageCollection& pages = document->GetPages();
		unsigned int pageCount = pages.GetCount();
		for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
			PdfPage& page = pages.GetPageAt(pageIndex);

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
					PdfDictionary& dictionary = annotation.GetDictionary();

					std::vector<PdfAppearanceIdentity> apperanceStreams;
					annotation.GetAppearanceStreams(apperanceStreams);

					PdfObject* apperanceStream = nullptr;

					unsigned int apperanceStreamCount = apperanceStreams.size();
					if (apperanceStreamCount == 1) {
						apperanceStream = const_cast<PdfObject*>(apperanceStreams[0].Object);
					}
					else {
						PdfName as = dictionary.FindKeyAs<PdfName>(PdfName("AS"));
						for (auto iterator = apperanceStreams.begin(); iterator.operator!=(apperanceStreams.end()); iterator.operator++()) {
							PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
							if (apperanceIdentity.Type == PdfAppearanceType::Normal && apperanceIdentity.State == as) {
								apperanceStream = const_cast<PdfObject*>(apperanceIdentity.Object);
							}
						}
					}

					if (apperanceStream == nullptr) {
						continue;
					}

					PdfObject& apperanceStreanRef = *apperanceStream;
					
					std::unique_ptr<PdfXObjectForm> xObjectUniquePtr;
					bool xObjectCreated = PdfXObjectForm::TryCreateFromObject(apperanceStreanRef, xObjectUniquePtr);
					if (!xObjectCreated) {
						continue;
					}

					PdfXObjectForm* xObject = xObjectUniquePtr.get();
					PdfXObjectForm& xObjectRef = *xObject;

					Rect rect = annotation.GetRect();
					double left = rect.GetLeft();
					double bottom = rect.GetBottom();

					PdfPainter* painter = new PdfPainter();
					painter->SetCanvas(page);
					painter->DrawXObject(xObjectRef, left, bottom);
					painter->FinishDrawing();
				}
			}
		}

		this->removeFormField(fieldName);

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