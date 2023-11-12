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

	PiPiEditor* PiPiEditor::flatten(std::string fieldName) {
		PdfMemDocument* document = this->document;

		std::vector<const PdfAnnotation*>* annotations = PiPiUtil::SearchAnnotation(document, fieldName);
		
		for (auto iterator = annotations->begin(); iterator != annotations->end(); ++iterator) {
			const PdfAnnotation* constAnnotation = *iterator;
			PdfAnnotation* annotation = const_cast<PdfAnnotation*>(constAnnotation);

			PdfDictionary& dictionary = annotation->GetDictionary();

			std::vector<PdfAppearanceIdentity> apperanceStreams;
			annotation->GetAppearanceStreams(apperanceStreams);

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

			Rect rect = annotation->GetRect();
			double left = rect.GetLeft();
			double bottom = rect.GetBottom();

			PdfPage* page = annotation->GetPage();
			PdfPage& pageRef = *page;

			PdfPainter* painter = new PdfPainter();
			painter->SetCanvas(pageRef);
			painter->DrawXObject(xObjectRef, left, bottom);
			painter->FinishDrawing();

			delete painter;
		}

		delete annotations;

		PiPiUtil::RemoveField(document, fieldName);

		return this;
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