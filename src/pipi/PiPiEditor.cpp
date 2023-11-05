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

					size_t apperanceStreamCount = apperanceStreams.size();
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

		PiPiUtil::RemoveFieldFromAcroForm(document, fieldName);
		PiPiUtil::RemoveFieldFromPage(document, fieldName);

		return this;
	}

	PiPiEditor* PiPiEditor::removeFormField(std::string fieldName) {
		PdfMemDocument* document = this->document;

		PiPiUtil::RemoveFieldFromAcroForm(document, fieldName);
		PiPiUtil::RemoveFieldFromPage(document, fieldName);

		return this;
	}

	PiPiEditor* PiPiEditor::renameFormField(std::string oldFieldName, std::string newFieldName) {
		return this;
	}

	
}