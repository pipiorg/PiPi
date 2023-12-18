#include "PiPiFiller.h"

namespace PiPi {
	PiPiFiller::PiPiFiller(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAppearanceManager* appearanceManager, PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver) {
        this->document = document;
        this->appearanceManager = appearanceManager;
        this->fontManager = fontManager;
        this->fieldObserver = fieldObserver;
        this->annotObserver = annotObserver;
	}

	bool PiPiFiller::isOperable() {
		return this->document != nullptr;
	}

	PiPiFiller* PiPiFiller::fillValue(std::string name, std::string value) {
		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = this->fontManager;
        PiPiAppearanceManager* appearanceManager = this->appearanceManager;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;

		std::set<PdfField*>* fields = PiPiFieldUtil::SearchField(fieldObserver, document, name);

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

        appearanceManager->MarkNeedAppearance(name);

		return this;
	}

    PiPiFiller* PiPiFiller::fillValue(std::string fieldName, std::string value, bool ellipsis) {
        PdfMemDocument* document = this->document;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        
        if (ellipsis) {
            std::set<PdfAnnotation*>* annots =  PiPiAnnotationUtil::SearchFieldAnnotation(annotObserver, document, fieldName);
            
            if (annots->size() > 0) {
                PdfAnnotation* minAnnot = nullptr;
                float minAnnotWidthWithoutBorder = 0;
                float minAnnotHeightWithoutBorder = 0;
                
                for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++()) {
                    PdfAnnotation* annot = *iterator;
                    
                    float annotWidth = PiPiExtractUtil::ExtractAnnotationWidth(annot);
                    float annotHeight = PiPiExtractUtil::ExtractAnnotationHeight(annot);
                    
                    float annotBorderWidth = PiPiExtractUtil::ExtractAnnotationBorderExists(annot)
                        ? PiPiExtractUtil::ExtractAnnotationBorderWidth(annot)
                        : 0;
                    
                    float annotWidthWithoutBorder = annotWidth - annotBorderWidth * 2;
                    float annotHeightWithoutBorder = annotHeight - annotBorderWidth * 2;
                    
                    float annotArea = annotWidthWithoutBorder * annotHeightWithoutBorder;
                    
                    if (minAnnot == nullptr) {
                        minAnnot = annot;
                        minAnnotWidthWithoutBorder = annotWidthWithoutBorder;
                        minAnnotHeightWithoutBorder = annotHeightWithoutBorder;
                        continue;
                    }
                    
                    float minAnnotArea = minAnnotWidthWithoutBorder * minAnnotHeightWithoutBorder;
                    
                    if (minAnnotArea > annotArea) {
                        minAnnot = annot;
                    }
                }
                
                bool multiline = PiPiExtractUtil::ExtractAnnotationTextMultiine(minAnnot);
                std::string fontName = PiPiExtractUtil::ExtractAnnotationFontName(minAnnot);
                float fontSize = PiPiExtractUtil::ExtractAnnotationFontSize(minAnnot);
                
                value = multiline
                    ? this->ellipsisValueMultiline(value, minAnnotWidthWithoutBorder, minAnnotHeightWithoutBorder, fontName, fontSize)
                    : this->ellipsisValue(value, minAnnotWidthWithoutBorder, minAnnotHeightWithoutBorder, fontName, fontSize);
            }
            
            delete annots;
        }
        
        return this->fillValue(fieldName, value);
    }

	PiPiFiller* PiPiFiller::fillImage(std::string fieldName, char* imageBytes, size_t imageSize) {
		PdfMemDocument* document = this->document;
        PiPiAnnotationObserver* annotObserver = this->annotObserver;
        PiPiFieldObserver* fieldObserver = this->fieldObserver;
		
		std::set<PdfAnnotation*>* annotations = PiPiAnnotationUtil::SearchFieldAnnotation(annotObserver, document, fieldName);
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

		PiPiFieldUtil::RemoveField(fieldObserver, annotObserver, document, fieldName);

		return this;
	}

    std::string PiPiFiller::ellipsisValue(std::string value, float width, float height, std::string fontName, float fontSize) {
        PiPiFontManager* fontManager = this->fontManager;
        
        const PdfFont* font = fontManager->accessFont(fontName);
        
        const PdfFontMetrics& fontMetricsRef = font->GetMetrics();
        const PdfFontMetrics* fontMetrics = &fontMetricsRef;
        
        PdfTextState textState;
        textState.Font = font;
        textState.FontSize = fontSize;
        textState.FontScale = 1.0;
        textState.RenderingMode = PdfTextRenderingMode::Fill;
        
        double fLineWidth = font->GetStringLength(value, textState);
        if (fLineWidth < width) {
            return value;
        }
        
        
        std::string line = "";
        std::string nextLine = "";
        
        auto iterator = value.begin();
        while (iterator != value.end()) {
            utf8::utfchar32_t character = utf8::next(iterator, value.end());
            
            utf8::append(character, nextLine);
            
            std::string nextLineWithEllipsis = nextLine + "...";
            
            double lineWidth = font->GetStringLength(nextLineWithEllipsis, textState);
            if (lineWidth > width) {
                line += "...";
                break;
            }
            
            utf8::append(character, line);
        }
        
        return line;
    }

    std::string PiPiFiller::ellipsisValueMultiline(std::string value, float width, float height, std::string fontName, float fontSize) {
        PiPiFontManager* fontManager = this->fontManager;
        
        const PdfFont* font = fontManager->accessFont(fontName);
        
        const PdfFontMetrics& fontMetricsRef = font->GetMetrics();
        const PdfFontMetrics* fontMetrics = &fontMetricsRef;
        
        PdfTextState textState;
        textState.Font = font;
        textState.FontSize = fontSize;
        textState.FontScale = 1.0;
        textState.RenderingMode = PdfTextRenderingMode::Fill;
        
        // 切割每行有哪些字
        std::vector<std::string>* lines = new std::vector<std::string>();
        
        std::string line = "";
        std::string nextLine = "";

        auto iterator = value.begin();
        while (iterator != value.end()) {
            utf8::utfchar32_t character = utf8::next(iterator, value.end());
            
            utf8::append(character, nextLine);
            
            double lineWidth = font->GetStringLength(nextLine, textState);
            if (lineWidth > width) {
                lines->push_back(line);
                
                line = "";
                nextLine = "";
                
                utf8::append(character, line);
                utf8::append(character, nextLine);
                
                continue;
            }
            
            utf8::append(character, line);
        }
        
        if (line.length() != 0) {
            lines->push_back(line);
        }
        
        // 計算單行高度
        double leading = fontMetrics->GetLeading();
        double lineSpacing = font->GetLineSpacing(textState);
        double descent = font->GetDescent(textState);
        double signalLineHeight = lineSpacing + leading;
        
        // 計算可塞下幾行
        double lineHieght = leading;
        size_t availableLineCount = 0;
        size_t lineCount = lines->size();
        while(lineCount--) {
            if (lineHieght + signalLineHeight > height) {
                break;
            }
            
            lineHieght += signalLineHeight;
            availableLineCount++;
        }
        
        // 開始湊完整值
        value = "";
        for (size_t i = 0; i < availableLineCount; i++) {
            if (i == availableLineCount - 1) {
                // 做 ...
                std::string lastLine = (*lines)[i];
                std::string newLastLine = "";
                std::string postNewLastLine = "";
                auto iterator = lastLine.begin();
                while (iterator != lastLine.end()) {
                    utf8::utfchar32_t character = utf8::next(iterator, lastLine.end());
                    
                    utf8::append(character, postNewLastLine);
                    
                    std::string postNewLastLineWithEllipsis = postNewLastLine + "...";
                    
                    double lastLineWidth = font->GetStringLength(postNewLastLineWithEllipsis, textState);
                    if (lastLineWidth > width) {
                        newLastLine += "...";
                        break;
                    }
                    
                    utf8::append(character, newLastLine);
                }
                
                value += newLastLine;
                break;
            }
            
            value += (*lines)[i];
        }
        
        return value;
    }
}
