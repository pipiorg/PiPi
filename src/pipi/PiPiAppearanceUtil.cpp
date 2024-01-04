#include "PiPiAppearanceUtil.h"

namespace PiPi {
    void PiPiAppearanceUtil::FlattenAppearance(PiPiFontManager *fontManager, PiPiFieldManager* fieldManager, PdfAnnotation *annot) {
        GenerateAppearance(fontManager, fieldManager, annot);
        
        PdfDictionary& dictionary = annot->GetDictionary();

        std::vector<PdfAppearanceIdentity> apperanceStreams;
        annot->GetAppearanceStreams(apperanceStreams);

        PdfObject* apperanceStream = nullptr;

        size_t apperanceStreamCount = apperanceStreams.size();
        if (apperanceStreamCount == 1) {
            apperanceStream = const_cast<PdfObject*>(apperanceStreams[0].Object);
        }
        else {
            PdfName as = dictionary.HasKey(PdfName("AS")) ? dictionary.FindKeyAsSafe<PdfName>(PdfName("AS")) : PdfName("Off");
            
            unsigned int normalCount = 0;
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                if (apperanceIdentity.Type == PdfAppearanceType::Normal) {
                    normalCount++;
                }
            }
            
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                
                if (apperanceIdentity.Type == PdfAppearanceType::Normal && apperanceIdentity.State == as) {
                    apperanceStream = const_cast<PdfObject*>(apperanceIdentity.Object);
                }
            }
        }

        if (apperanceStream == nullptr) {
            throw PiPiAppearanceException(PiPiAppearanceException::PiPiAppearanceExceptionCode::NoAppearanceExistsForFlatten);
        }

        PdfObject& apperanceStreanRef = *apperanceStream;

        std::unique_ptr<PdfXObjectForm> xObjectUniquePtr;
        bool xObjectCreated = PdfXObjectForm::TryCreateFromObject(apperanceStreanRef, xObjectUniquePtr);
        if (!xObjectCreated) {
            throw PiPiAppearanceException(PiPiAppearanceException::PiPiAppearanceExceptionCode::CreateFormObjectFromAppearanceFail);
        }

        PdfXObjectForm* xObject = xObjectUniquePtr.get();

        Rect rect = annot->GetRect();
        double left = rect.GetLeft();
        double bottom = rect.GetBottom();

        PdfPage* page = annot->GetPage();

        PdfPainter* painter = new PdfPainter(PdfPainterFlags::NoSaveRestorePrior);

        painter->SetCanvas(*page);
        painter->SetClipRect(rect);
        painter->DrawXObject(*xObject, left, bottom);
        painter->FinishDrawing();

        delete painter;
    }

	void PiPiAppearanceUtil::GenerateAppearance(PiPiFontManager* fontManager, PiPiFieldManager* fieldManager, PdfAnnotation* annot) {
        PiPiFieldType type = PiPiExtractUtil::ExtractAnnotationType(annot);

		switch (type) {
            case PiPiFieldType::TextBox:
                GenerateTextBoxAppearance(fontManager, annot);
                break;
            case PiPiFieldType::CheckBox:
                ClearCheckBoxAppearance(annot);
                GenerateCheckBoxAppearance(annot);
                break;
            case PiPiFieldType::Unknown:
            default:
                throw PiPiAppearanceException(PiPiAppearanceException::PiPiAppearanceExceptionCode::UnsupportedFieldType);
		}
	}

	void PiPiAppearanceUtil::ClearCheckBoxAppearance(PdfAnnotation* annot) {
		PdfDictionary* dict = &(annot->GetDictionary());

		PdfObject* apObj = dict->FindKey(PdfName("AP"));
        if (apObj == nullptr) {
            return;
        }
        
		PdfDictionary* ap = &(apObj->GetDictionary());
		
		PdfObject* nObj = ap->FindKey(PdfName("N"));
		if (nObj != nullptr) {
			PdfDictionary* n = &(nObj->GetDictionary());
			n->Clear();
		}

		PdfObject* dObj = ap->FindKey(PdfName("D"));
		if (nObj != nullptr) {
			PdfDictionary* d = &(dObj->GetDictionary());
			d->Clear();
		}
	}

	void PiPiAppearanceUtil::GenerateCheckBoxAppearance(PdfAnnotation* annot) {
		PiPiAppearanceUtil::GenerateCheckBoxDownCheckAppearance(annot);
		PiPiAppearanceUtil::GenerateCheckBoxDownUnCheckAppearance(annot);
		PiPiAppearanceUtil::GenerateCheckBoxNormalCheckAppearance(annot);
	}

	void PiPiAppearanceUtil::GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot) {
		PdfDocument* document = &(annot->GetDocument());

		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();

		PdfPainter* painter = new PdfPainter();
        
        PdfGraphicsStateWrapper* graphicsState = &(painter->GraphicsState);

		painter->SetCanvas(*xObject);
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

		annot->SetAppearanceStream(*xObject, PdfAppearanceType::Normal, PdfName("Yes"));
	}

	void PiPiAppearanceUtil::GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot) {
		PdfDocument* document = &(annot->GetDocument());

		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();

		PdfPainter* painter = new PdfPainter();
        
        PdfGraphicsStateWrapper* graphicsState = &(painter->GraphicsState);

		painter->SetCanvas(*xObject);
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

		annot->SetAppearanceStream(*xObject, PdfAppearanceType::Down, PdfName("Yes"));
	}

	void PiPiAppearanceUtil::GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot) {
		PdfDocument* document = &(annot->GetDocument());

		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();

		PdfPainter* painter = new PdfPainter();
        
        PdfGraphicsStateWrapper* graphicsState = &(painter->GraphicsState);

		painter->SetCanvas(*xObject);
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

		annot->SetAppearanceStream(*xObject, PdfAppearanceType::Down, PdfName("Off"));
	}

	void PiPiAppearanceUtil::GenerateTextBoxAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot) {
		PdfDocument* document = &(annot->GetDocument());
        
		Rect annotRect = annot->GetRect();

		double width = annotRect.Width;
		double height = annotRect.Height;

        Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();
        
		PdfPainter* painter = new PdfPainter();
        
		painter->SetCanvas(*xObject);
		painter->SetClipRect(xObjectRect);
        
        PdfGraphicsStateWrapper* graphicsState = &(painter->GraphicsState);
        
        PdfTextStateWrapper* textState = &(painter->TextState);
        
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
		
		const PdfFont* font = fontManager->AccessFont(fontName) == nullptr
            ? fontManager->AccessDefaultFont()
            : fontManager->AccessFont(fontName);
		const PdfFont& fontRef = *font;
		
		textState->SetFont(fontRef, fontSize);

        std::string sText = PiPiExtractUtil::ExtractAnnotationValue(annot);
        
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
        
        painter->Restore();

		painter->FinishDrawing();

		delete painter;

		annot->SetAppearanceStream(*xObject);
	}
}
