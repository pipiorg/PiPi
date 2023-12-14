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

		double cx = width / 2;
		double cy = height / 2;

		double size = std::min(width, height) / 2;

		double p2x = -1 + 0.75;
		double p2y = -1 + 0.51;

		double p3y = 1 - 0.525;
		double p3x = 1 - 0.31;

		double p1x = -1 + 0.325;
		double p1y = -((p1x - p2x) * (p3x - p2x)) / (p3y - p2y) + p2y;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();
		PdfXObjectForm& xObjectRef = *xObject;

		PdfPainter* painter = new PdfPainter();

		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);

		painter->GraphicsState.SetLineWidth(1.5);
		painter->GraphicsState.SetStrokeColor(PdfColor(0, 0, 0));
		
		PdfPainterPath path;
		path.MoveTo(cx + p1x * size, cy + p1y * size);
		path.AddLineTo(cx + p2x * size, cy + p2y * size);
		path.AddLineTo(cx + p3x * size, cy + p3y * size);

		painter->DrawPath(path, PdfPathDrawMode::Stroke);

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

		double cx = width / 2;
		double cy = height / 2;

		double size = std::min(width, height) / 2;

		double p2x = -1 + 0.75;
		double p2y = -1 + 0.51;

		double p3y = 1 - 0.525;
		double p3x = 1 - 0.31;

		double p1x = -1 + 0.325;
		double p1y = -((p1x - p2x) * (p3x - p2x)) / (p3y - p2y) + p2y;

		Rect xObjectRect = Rect(0, 0, width, height);

		std::unique_ptr<PdfXObjectForm> xObjectPtr = document->CreateXObjectForm(xObjectRect);
		PdfXObjectForm* xObject = xObjectPtr.get();
		PdfXObjectForm& xObjectRef = *xObject;

		PdfPainter* painter = new PdfPainter();

		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);

		PdfGraphicsStateWrapper& graphicsState = painter->GraphicsState;
		PdfColor* bgColor = new PdfColor(0.75, 0.75, 0.75);
		PdfColor& bgColorRef = *bgColor;
		graphicsState.SetFillColor(bgColorRef);

		painter->DrawRectangle(xObjectRect, PdfPathDrawMode::Fill);

		painter->GraphicsState.SetLineWidth(1.5);
		painter->GraphicsState.SetStrokeColor(PdfColor(0, 0, 0));

		PdfPainterPath path;
		path.MoveTo(cx + p1x * size, cy + p1y * size);
		path.AddLineTo(cx + p2x * size, cy + p2y * size);
		path.AddLineTo(cx + p3x * size, cy + p3y * size);

		painter->DrawPath(path, PdfPathDrawMode::Stroke);

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

		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);

		PdfGraphicsStateWrapper& graphicsState = painter->GraphicsState;
		PdfColor* bgColor = new PdfColor(0.75, 0.75, 0.75);
		PdfColor& bgColorRef = *bgColor;
		graphicsState.SetFillColor(bgColorRef);

		painter->DrawRectangle(xObjectRect, PdfPathDrawMode::Fill);

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

		PdfGraphicsStateWrapper& graphicsStateRef = painter->GraphicsState;
		PdfGraphicsStateWrapper* graphicsState = &graphicsStateRef;

		PdfTextStateWrapper& textStateRef = painter->TextState;
		PdfTextStateWrapper* textState = &textStateRef;

		painter->SetCanvas(xObjectRef);
		painter->SetClipRect(xObjectRect);

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
            
            bool multiline = PiPiExtractUtil::ExtractAnnotationTextMultiine(annot);
            PiPiTextHorizontalAlignment horizontalAlignment = PiPiExtractUtil::ExtractAnnotationTextHorizontalAlignment(annot);
            
            
            if (multiline) {
                PdfDrawTextMultiLineParams multineParams;
                
                multineParams.VerticalAlignment = PdfVerticalAlignment::Center;

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
                
                painter->DrawTextMultiLine(sText, 0, 0, width, height, multineParams);
            } else {
                painter->DrawText(sText, 0, 0);
            }
		}

		painter->FinishDrawing();

		delete painter;

		annot->SetAppearanceStream(xObjectRef);
	}
}
