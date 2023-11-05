#include "PiPiFiller.h"

namespace PiPi {
	PiPiFiller::PiPiFiller(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiFiller::init(PdfMemDocument* document) {
		this->document = document;
	}

	PiPiFiller* PiPiFiller::fillValue(std::string targetName, std::string value) {
		PdfMemDocument* document = this->document;
		PdfAcroForm* acroForm = document->GetAcroForm();

		acroForm->SetNeedAppearances(true);
		acroForm->GetFieldCount();
		for (auto iterator = acroForm->begin(); iterator.operator!=(acroForm->end()); iterator.operator++()) {
			PdfField* field = iterator.operator*();

			nullable<const PdfString&> nameObject = field->GetName();
			if (!nameObject.has_value()) {
				continue;
			}

			std::string name = nameObject->GetString();

			if (name == targetName) {
				PdfFieldType fieldType = field->GetType();
				
				if (fieldType == PdfFieldType::TextBox) {
					PdfTextBox* textBoxField = (PdfTextBox*)field;
					PdfString valueString(value);
					textBoxField->SetText(valueString);
				}
				else if (fieldType == PdfFieldType::CheckBox) {
					PdfCheckBox* checkBoxField = (PdfCheckBox*)field;

					bool isCheckBox = checkBoxField->IsCheckBox();
					if (isCheckBox) {
						bool checked = value == "Yes" || value == "On";
						checkBoxField->SetChecked(checked);
					}
				}
			}
		}

		document->CollectGarbage();

		return this;
	}

	PiPiFiller* PiPiFiller::fillImage(std::string fieldName, char* imageBytes, size_t imageSize) {
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
					Rect annotRect = annotation.GetRect();

					double annotBottom = annotRect.GetBottom();
					double annotTop = annotRect.GetTop();
					double annotLeft = annotRect.GetLeft();
					double annotRight = annotRect.GetRight();

					double annotWidth = annotRight - annotLeft;
					double annotHeight = annotTop - annotBottom;

					std::unique_ptr<PdfImage> imageUniquePtr = document->CreateImage();
					PdfImage* image = imageUniquePtr.get();
					image->LoadFromBuffer(bufferview(imageBytes, imageSize));
					PdfImage& imageRef = *image;

					Rect imageRect = image->GetRect();
					
					double imageBottom = imageRect.GetBottom();
					double imageTop = imageRect.GetTop();
					double imageLeft = imageRect.GetLeft();
					double imageRight = imageRect.GetRight();
					
					double imageWidth = imageRight - imageLeft;
					double imageHeight = imageTop - imageBottom;

					double scale = std::min(annotWidth / imageWidth, annotHeight / imageHeight);

					double scaledImageWidth = imageWidth * scale;
					double scaledImageHeight = imageHeight * scale;

					double left = annotLeft + annotWidth / 2 - scaledImageWidth / 2;
					double bottom = annotBottom + annotHeight / 2 - scaledImageHeight / 2;

					PdfPainter* painter = new PdfPainter();
					painter->SetCanvas(page);
					painter->DrawImage(imageRef, left, bottom, scale, scale);
					painter->FinishDrawing();
				}
			}
		}

		PiPiUtil::RemoveFieldFromAcroForm(document, fieldName);
		PiPiUtil::RemoveFieldFromPage(document, fieldName);

		return this;
	}
}