#include "PiPiFiller.h"

namespace PiPi {
	PiPiFiller::PiPiFiller(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiFiller::init(PdfMemDocument* document) {
		this->document = document;
	}

	bool PiPiFiller::isOperable() {
		return this->document != nullptr;
	}

	PiPiFiller* PiPiFiller::fillValue(std::string name, std::string value) {
		PdfMemDocument* document = this->document;

		PdfAcroForm* acroForm = document->GetAcroForm();
		acroForm->SetNeedAppearances(true);

		std::vector<const PdfField*>* fields = PiPiUtil::SearchField(document, name);

		if (fields == nullptr) {
			return this;
		}

		for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
			const PdfField* field = iterator.operator*();

			PdfFieldType type = field->GetType();
			if (type == PdfFieldType::TextBox) {
				PdfTextBox* textBoxField = (PdfTextBox*)field;
				PdfString valueString(value);
				textBoxField->SetText(valueString);
			} else if (type == PdfFieldType::CheckBox) {
				PdfCheckBox* checkBoxField = (PdfCheckBox*)field;

				bool isCheckBox = checkBoxField->IsCheckBox();
				if (!isCheckBox) continue;

				bool checked = value == "Yes" || value == "On";
				checkBoxField->SetChecked(checked);
			}
		}

		delete fields;

		return this;
	}

	PiPiFiller* PiPiFiller::fillImage(std::string fieldName, char* imageBytes, size_t imageSize) {
		PdfMemDocument* document = this->document;
		
		std::vector<const PdfAnnotation*>* annotations = PiPiUtil::SearchAnnotation(document, fieldName);
		for (auto iterator = annotations->begin(); iterator != annotations->end(); ++iterator) {
			const PdfAnnotation* annotation = *iterator;

			const PdfPage* constPage = annotation->GetPage();
			PdfPage* page = const_cast<PdfPage*>(constPage);
			PdfPage& pageRef = *page;

			Rect annotRect = annotation->GetRect();

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
			painter->SetCanvas(pageRef);
			painter->DrawImage(imageRef, left, bottom, scale, scale);
			painter->FinishDrawing();
			
			delete painter;
		}

		delete annotations;

		PiPiUtil::RemoveField(document, fieldName);

		return this;
	}
}