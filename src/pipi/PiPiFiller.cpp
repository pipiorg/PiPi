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

	PiPiFiller* PiPiFiller::fillImage(std::string fieldName, char* imageBytes) {
		PdfMemDocument* document = this->document;
		PdfAcroForm* acroForm = document->GetAcroForm();

		PdfField& field = acroForm->CreateField(fieldName, PdfFieldType::PushButton);

		acroForm->SetNeedAppearances(true);

		return this;
	}
}