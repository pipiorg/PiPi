#include "PiPiAppearanceUtil.h"

namespace PiPi {
	void PiPiAppearanceUtil::GenerateAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot) {
		PdfObject& annotObjRef = annot->GetObject();

		std::unique_ptr<PdfField> fieldPtr;
		bool created = PdfField::TryCreateFromObject(annotObjRef, fieldPtr);
		if (!created) {
			return;
		}

		PdfField* field = fieldPtr.get();

		PdfFieldType type = field->GetType();

		switch (type) {
			case PdfFieldType::CheckBox:
				ClearCheckBoxAppearance(annot, (PdfCheckBox*)field);
				GenerateCheckBoxAppearance(annot, (PdfCheckBox*)field);
				break;
			case PdfFieldType::TextBox:
				GenerateTextBoxAppearance(fontManager, annot, (PdfTextBox*)field);
				break;
		}
	}

	void PiPiAppearanceUtil::ClearCheckBoxAppearance(PdfAnnotation* annot, PdfCheckBox* field) {
		PdfDictionary& dictRef = annot->GetDictionary();
		PdfDictionary* dict = &dictRef;

		PdfObject* apObj = dict->FindKey(PdfName("AP"));
		PdfDictionary& apRef = apObj->GetDictionary();
		PdfDictionary* ap = &apRef;
		
		PdfObject* nObj = ap->FindKey(PdfName("N"));
		if (nObj != nullptr) {
			PdfDictionary& nRef = nObj->GetDictionary();
			PdfDictionary* n = &nRef;
			n->Clear();
		}

		PdfObject* dObj = ap->FindKey(PdfName("D"));
		if (nObj != nullptr) {
			PdfDictionary& dRef = dObj->GetDictionary();
			PdfDictionary* d = &dRef;
			d->Clear();
		}
	}

	void PiPiAppearanceUtil::GenerateCheckBoxAppearance(PdfAnnotation* annot, PdfCheckBox* field) {
		PiPiAppearanceUtil::GenerateCheckBoxDownCheckAppearance(annot, field);
		PiPiAppearanceUtil::GenerateCheckBoxDownUnCheckAppearance(annot, field);
		PiPiAppearanceUtil::GenerateCheckBoxNormalCheckAppearance(annot, field);
	}

	void PiPiAppearanceUtil::GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot, PdfCheckBox* field) {
		PdfDocument& documentRef = annot->GetDocument();
		PdfDocument* document = &documentRef;

		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();
		PdfXObjectForm& xObjectRef = *xObject;

		PdfPainter* painter = new PdfPainter();
        
        PdfGraphicsStateWrapper& graphicsStateRef = painter->GraphicsState;
        PdfGraphicsStateWrapper* graphicsState = &graphicsStateRef;

		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);
        
        // 畫邊框
        double borderWidth = 0;
        
        bool borderExists = PiPiExtractUtil::ExtractAnnotationBorderExists(annot);
        if (borderExists) {
            painter->Save();
            
            float borderColorRed = 0;
            float borderColorGreen = 0;
            float borderColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);
            
            borderWidth = PiPiExtractUtil::ExtractAnnotationBorderWidth(annot);
            
            graphicsState->SetLineWidth(borderWidth);
            graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
            graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);
            
            painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);
            
            painter->Restore();
        }
        
        // 畫背景
        bool backgroundExists = PiPiExtractUtil::ExtractAnnotationBackgroundExists(annot);
        if (backgroundExists) {
            painter->Save();
            
            float backgroundColorRed = 0;
            float backgroundColorGreen = 0;
            float backgroundColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);
            
            PdfColor* bgColor = new PdfColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue);
            PdfColor& bgColorRef = *bgColor;
            graphicsState->SetFillColor(bgColorRef);
            
            painter->DrawRectangle(borderWidth, borderWidth, width - borderWidth * 2, height - borderWidth * 2, PdfPathDrawMode::Fill);
            
            painter->Restore();
        }

        // 畫勾勾
        painter->Save();
        
        float colorRed = 0;
        float colorGreen = 0;
        float colorBlue = 0;
        PiPiExtractUtil::ExtractAnnotationColor(annot, &colorRed, &colorGreen, &colorBlue);
        
		graphicsState->SetLineWidth(1.5);
		graphicsState->SetStrokeColor(PdfColor(colorRed, colorGreen, colorBlue));
		
        double cx = width / 2;
        double cy = height / 2;

        double size = std::min(width - borderWidth * 2, height - borderWidth * 2) / 2;

        double p2x = -1 + 0.75;
        double p2y = -1 + 0.51;

        double p3y = 1 - 0.525;
        double p3x = 1 - 0.31;

        double p1x = -1 + 0.325;
        double p1y = -((p1x - p2x) * (p3x - p2x)) / (p3y - p2y) + p2y;
        
		PdfPainterPath path;
		path.MoveTo(cx + p1x * size, cy + p1y * size);
		path.AddLineTo(cx + p2x * size, cy + p2y * size);
		path.AddLineTo(cx + p3x * size, cy + p3y * size);

		painter->DrawPath(path, PdfPathDrawMode::Stroke);

        painter->Restore();
        
		painter->FinishDrawing();

		delete painter;

		annot->SetAppearanceStream(xObjectRef, PdfAppearanceType::Normal, PdfName("Yes"));
	}

	void PiPiAppearanceUtil::GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot, PdfCheckBox* field) {
		PdfDocument& documentRef = annot->GetDocument();
		PdfDocument* document = &documentRef;

		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();
		PdfXObjectForm& xObjectRef = *xObject;

		PdfPainter* painter = new PdfPainter();
        
        PdfGraphicsStateWrapper& graphicsStateRef = painter->GraphicsState;
        PdfGraphicsStateWrapper* graphicsState = &graphicsStateRef;

		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);

        // 畫邊框
        double borderWidth = 0;
        
        bool borderExists = PiPiExtractUtil::ExtractAnnotationBorderExists(annot);
        if (borderExists) {
            painter->Save();
            
            borderWidth = PiPiExtractUtil::ExtractAnnotationBorderWidth(annot);
            
            float borderColorRed = 0;
            float borderColorGreen = 0;
            float borderColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);
            
            graphicsState->SetLineWidth(borderWidth);
            graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
            graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);
            
            painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);
            
            painter->Restore();
        }
        
        // 畫背景
        bool backgroundExists = PiPiExtractUtil::ExtractAnnotationBackgroundExists(annot);
        if (backgroundExists) {
            painter->Save();
            
            float backgroundColorRed = 0;
            float backgroundColorGreen = 0;
            float backgroundColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);
            
            float invertBackgroundColorRed = 1.0 - backgroundColorRed;
            float invertBackgroundColorGreen = 1.0 - backgroundColorBlue;
            float invertBackgroundColorBlue = 1.0 - backgroundColorBlue;
            
            PdfColor* bgColor = new PdfColor(invertBackgroundColorRed, invertBackgroundColorGreen, invertBackgroundColorBlue);
            PdfColor& bgColorRef = *bgColor;
            graphicsState->SetFillColor(bgColorRef);

            painter->DrawRectangle(borderWidth, borderWidth, width - borderWidth * 2, height - borderWidth * 2, PdfPathDrawMode::Fill);
            
            painter->Restore();
        }
        
        // 畫勾勾
        painter->Save();
        
        float colorRed = 0;
        float colorGreen = 0;
        float colorBlue = 0;
        PiPiExtractUtil::ExtractAnnotationColor(annot, &colorRed, &colorGreen, &colorBlue);
        
        float invertColorRed = 1.0 - colorRed;
        float invertColorGreen = 1.0 - colorGreen;
        float invertColorBlue = 1.0 - colorBlue;
        
		graphicsState->SetLineWidth(1.5);
		graphicsState->SetStrokeColor(PdfColor(invertColorRed, invertColorGreen, invertColorBlue));

        double cx = width / 2;
        double cy = height / 2;

        double size = std::min(width - borderWidth * 2, height - borderWidth * 2) / 2;

        double p2x = -1 + 0.75;
        double p2y = -1 + 0.51;

        double p3y = 1 - 0.525;
        double p3x = 1 - 0.31;

        double p1x = -1 + 0.325;
        double p1y = -((p1x - p2x) * (p3x - p2x)) / (p3y - p2y) + p2y;
        
		PdfPainterPath path;
		path.MoveTo(cx + p1x * size, cy + p1y * size);
		path.AddLineTo(cx + p2x * size, cy + p2y * size);
		path.AddLineTo(cx + p3x * size, cy + p3y * size);

		painter->DrawPath(path, PdfPathDrawMode::Stroke);

        painter->Restore();
        
		painter->FinishDrawing();

		delete painter;

		annot->SetAppearanceStream(xObjectRef, PdfAppearanceType::Down, PdfName("Yes"));
	}

	void PiPiAppearanceUtil::GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot, PdfCheckBox* field) {
		PdfDocument& documentRef = annot->GetDocument();
		PdfDocument* document = &documentRef;

		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();
		PdfXObjectForm& xObjectRef = *xObject;

		PdfPainter* painter = new PdfPainter();
        
        PdfGraphicsStateWrapper& graphicsStateRef = painter->GraphicsState;
        PdfGraphicsStateWrapper* graphicsState = &graphicsStateRef;

		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);

        // 畫邊框
        double borderWidth = 0;
        
        bool borderExists = PiPiExtractUtil::ExtractAnnotationBorderExists(annot);
        if (borderExists) {
            painter->Save();
            
            borderWidth = PiPiExtractUtil::ExtractAnnotationBorderWidth(annot);
            
            float borderColorRed = 0;
            float borderColorGreen = 0;
            float borderColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);
            
            graphicsState->SetLineWidth(borderWidth);
            graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
            graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);
            
            painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);
            
            painter->Restore();
        }
        
        // 畫背景
        bool backgroundExists = PiPiExtractUtil::ExtractAnnotationBackgroundExists(annot);
        if (backgroundExists) {
            painter->Save();
            
            float backgroundColorRed = 0;
            float backgroundColorGreen = 0;
            float backgroundColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);
            
            float invertBackgroundColorRed = 1.0 - backgroundColorRed;
            float invertBackgroundColorGreen = 1.0 - backgroundColorBlue;
            float invertBackgroundColorBlue = 1.0 - backgroundColorBlue;
            
            PdfColor* bgColor = new PdfColor(invertBackgroundColorRed, invertBackgroundColorGreen, invertBackgroundColorBlue);
            PdfColor& bgColorRef = *bgColor;
            graphicsState->SetFillColor(bgColorRef);

            painter->DrawRectangle(borderWidth, borderWidth, width - borderWidth * 2, height - borderWidth * 2, PdfPathDrawMode::Fill);
            
            painter->Restore();
        }
        
		painter->FinishDrawing();

		delete painter;

		annot->SetAppearanceStream(xObjectRef, PdfAppearanceType::Down, PdfName("Off"));
	}

	void PiPiAppearanceUtil::GenerateTextBoxAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot, PdfTextBox* field) {
		PdfDocument& documentRef = annot->GetDocument();
		PdfDocument* document = &documentRef;
        
		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

        Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();
		PdfXObjectForm& xObjectRef = *xObject;
        
		PdfPainter* painter = new PdfPainter();
        
		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);
        
        PdfGraphicsStateWrapper& graphicsStateRef = painter->GraphicsState;
        PdfGraphicsStateWrapper* graphicsState = &graphicsStateRef;
        
        PdfTextStateWrapper& textStateRef = painter->TextState;
        PdfTextStateWrapper* textState = &textStateRef;
        
        // 畫邊框
        double borderWidth = 0;
        
        bool borderExists = PiPiExtractUtil::ExtractAnnotationBorderExists(annot);
        if (borderExists) {
            painter->Save();
            
            borderWidth = PiPiExtractUtil::ExtractAnnotationBorderWidth(annot);
            
            float borderColorRed = 0;
            float borderColorGreen = 0;
            float borderColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);
            
            graphicsState->SetLineWidth(borderWidth);
            graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
            graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);
            
            painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);
            
            painter->Restore();
        }
        
        // 畫背景
        bool backgroundExists = PiPiExtractUtil::ExtractAnnotationBackgroundExists(annot);
        if (backgroundExists) {
            painter->Save();
            
            float backgroundColorRed = 0;
            float backgroundColorGreen = 0;
            float backgroundColorBlue = 0;
            PiPiExtractUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);
            
            graphicsState->SetFillColor(PdfColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue));

            painter->DrawRectangle(borderWidth, borderWidth, width - borderWidth * 2, height - borderWidth * 2, PdfPathDrawMode::Fill);
            
            painter->Restore();
        }
        
        // 畫文字
        painter->Save();
        
        float textColorRed = 0;
        float textColorGreen = 0;
        float textColorBlue = 0;
        PiPiExtractUtil::ExtractAnnotationColor(annot, &textColorRed, &textColorGreen, &textColorBlue);
        
        graphicsState->SetFillColor(PdfColor(textColorRed, textColorGreen, textColorBlue));
        graphicsState->SetStrokeColor(PdfColor(textColorRed, textColorGreen, textColorBlue));
        
		float fontSize = PiPiExtractUtil::ExtractAnnotationFontSize(annot);
		std::string fontName = PiPiExtractUtil::ExtractAnnotationFontName(annot);
		
		const PdfFont* font = fontName == ""
			? fontManager->accessDefaultFont()
			: fontManager->accessFont(fontName) == nullptr
				? fontManager->accessDefaultFont()
				: fontManager->accessFont(fontName);
		const PdfFont& fontRef = *font;
		
		textState->SetFont(fontRef, fontSize);

		nullable<const PdfString&> nullalbleTextRef = field->GetText();
		if (nullalbleTextRef.has_value()) {
			const PdfString& textRef = nullalbleTextRef.value();
			const PdfString* text = &textRef;

			std::string sText = text->GetString();
            
            // 為了垂直置中，不管多行單行都照多行套
            // 前面套印處以過濾掉會變多行的文字
            
            
            PdfDrawTextMultiLineParams multineParams;
            
            bool multiline = PiPiExtractUtil::ExtractAnnotationTextMultiine(annot);
            multineParams.VerticalAlignment = multiline
                ? PdfVerticalAlignment::Top
                : PdfVerticalAlignment::Center;
            
            PiPiTextHorizontalAlignment horizontalAlignment = PiPiExtractUtil::ExtractAnnotationTextHorizontalAlignment(annot);
            switch (horizontalAlignment) {
                case PiPiTextHorizontalAlignment::Left:
                    multineParams.HorizontalAlignment = PdfHorizontalAlignment::Left;
                    break;
                case PiPiTextHorizontalAlignment::Center:
                    multineParams.HorizontalAlignment = PdfHorizontalAlignment::Center;
                    break;
                case PiPiTextHorizontalAlignment::Right:
                    multineParams.HorizontalAlignment = PdfHorizontalAlignment::Right;
                    break;
            }
            
            painter->DrawTextMultiLine(sText, borderWidth, borderWidth, width - borderWidth * 2, height - borderWidth * 2, multineParams);
		}
        
        painter->Restore();

		painter->FinishDrawing();

		delete painter;

		annot->SetAppearanceStream(xObjectRef);
	}
}
