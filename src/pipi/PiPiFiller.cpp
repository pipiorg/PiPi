#include "PiPiFiller.h"

namespace PiPi {
	PiPiFiller::PiPiFiller(PdfMemDocument* document, PiPiFontManager* fontManager) {
		this->init(document, fontManager);
	}

	void PiPiFiller::init(PdfMemDocument* document, PiPiFontManager* fontManager) {
		this->document = document;
		this->fontManager = fontManager;
	}

	bool PiPiFiller::isOperable() {
		return this->document != nullptr;
	}

	PiPiFiller* PiPiFiller::fillValue(std::string name, std::string value) {
		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = this->fontManager;

		std::vector<PdfField*>* fields = PiPiUtil::SearchField(document, name);

		for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;

			PdfFieldType type = field->GetType();
			if (type != PdfFieldType::CheckBox && type != PdfFieldType::TextBox) {
				return this;
			}

			if (type == PdfFieldType::TextBox) {
				PdfTextBox* textBoxField = (PdfTextBox*)field;
				PdfString valueString(value);
				textBoxField->SetText(valueString);
			}
			else if (type == PdfFieldType::CheckBox) {
				PdfCheckBox* checkBoxField = (PdfCheckBox*)field;

				bool isCheckBox = checkBoxField->IsCheckBox();
				if (!isCheckBox) {
					continue;
				}

				bool checked = value == "Yes" || value == "On";
				checkBoxField->SetChecked(checked);
			}
		}

		delete fields;

		std::vector<PdfAnnotation*>* annots = PiPiUtil::SearchFieldAnnotation(document, name);

		for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++()) {
			PdfAnnotation* annot = *iterator;

			PiPiAppearanceUtil::GenerateAppearance(fontManager, annot);
		}

		delete annots;

		return this;
	}

    PiPiFiller* PiPiFiller::fillValue(std::string fieldName, std::string value, bool ellipsis) {
        PdfMemDocument* document = this->document;
        
        if (ellipsis) {
            std::vector<PdfAnnotation*>* annots =  PiPiUtil::SearchFieldAnnotation(document, fieldName);
            
            if (annots->size() > 0) {
                PdfAnnotation* minAnnot = nullptr;
                float minAnnotWidth = 0;
                float minAnnotHeight = 0;
                
                for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++()) {
                    PdfAnnotation* annot = *iterator;
                    
                    float annotWidth = PiPiExtractUtil::ExtractAnnotationWidth(annot);
                    float annotHeight = PiPiExtractUtil::ExtractAnnotationHeight(annot);
                    float annotArea = annotWidth * annotHeight;
                    
                    if (minAnnot == nullptr) {
                        minAnnot = annot;
                        minAnnotWidth = annotWidth;
                        minAnnotHeight = annotHeight;
                        continue;
                    }
                    
                    float minAnnotArea = minAnnotWidth * minAnnotHeight;
                    
                    if (minAnnotArea > annotArea) {
                        minAnnot = annot;
                    }
                }
                
                bool multiline = PiPiExtractUtil::ExtractAnnotationTextMultiine(minAnnot);
                std::string fontName = PiPiExtractUtil::ExtractAnnotationFontName(minAnnot);
                float fontSize = PiPiExtractUtil::ExtractAnnotationFontSize(minAnnot);
                
                value = multiline
                    ? this->ellipsisValueMultiline(value, minAnnotWidth, minAnnotHeight, fontName, fontSize)
                    : this->ellipsisValue(value, minAnnotWidth, minAnnotHeight, fontName, fontSize);
            }
            
            delete annots;
        }
        
        this->fillValue(fieldName, value);
    }

	PiPiFiller* PiPiFiller::fillImage(std::string fieldName, char* imageBytes, size_t imageSize) {
		PdfMemDocument* document = this->document;
		
		std::vector<PdfAnnotation*>* annotations = PiPiUtil::SearchFieldAnnotation(document, fieldName);
		for (auto iterator = annotations->begin(); iterator != annotations->end(); ++iterator) {
			PdfAnnotation* annotation = *iterator;

			PdfPage* page = annotation->GetPage();
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

    std::string PiPiFiller::ellipsisValue(std::string value, float width, float height, std::string fontName, float fontSize) {
        PiPiFontManager* fontManager = this->fontManager;
        
        const PdfFont* font = fontManager->accessFont(fontName);
        
        const PdfFontMetrics& fontMetricsRef = font->GetMetrics();
        const PdfFontMetrics* fontMetrics = &fontMetricsRef;
        
        double xHeight = fontMetrics->GetXHeight() * fontSize / 1000;
        double ascent = fontMetrics->GetAscent() * fontSize / 1000;
        double decent = fontMetrics->GetDescent() * fontSize / 1000;
        return value;
    }

    std::string PiPiFiller::ellipsisValueMultiline(std::string value, float width, float height, std::string fontName, float fontSize) {
        PiPiFontManager* fontManager = this->fontManager;
        
        const PdfFont* font = fontManager->accessFont(fontName);
        
        const PdfFontMetrics& fontMetricsRef = font->GetMetrics();
        const PdfFontMetrics* fontMetrics = &fontMetricsRef;
        
        PdfTextState textState;
        textState.Font = font;
        textState.FontSize = fontSize;
        
        // 計算單行高度
        double leading = fontMetrics->GetLeading() * fontSize;
        double ascent = fontMetrics->GetAscent() * fontSize;
        double decent = fontMetrics->GetDescent() * fontSize;
        double signleLineHeight = ascent + (-decent);
        
        // 切割每行有哪些字
        std::vector<std::string>* lines = new std::vector<std::string>();
        std::string line = "";
        
        for (auto iterator = value.begin(); iterator != value.end(); iterator.operator++()) {
            auto character = *iterator;
            
            line += character;
            
            double lineWidth = font->GetStringLength(line, textState);
            if (lineWidth > width) {
                line.pop_back();
                lines->push_back(line);
                line = character;
            }
        }
        
        return value;
    }
}
