#include "PiPiAppearanceUtil.h"

namespace PiPi {
	void PiPiAppearanceUtil::GenerateAppearance(PdfAnnotation* annot) {
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
				ClearCheckBoxAppearance(annot);
				GenerateCheckBoxAppearance(annot);
			case PdfFieldType::TextBox:
				GenerateTextBoxAppearance(annot);
		}
	}

	void PiPiAppearanceUtil::ClearCheckBoxAppearance(PdfAnnotation* annot) {
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

	void PiPiAppearanceUtil::GenerateCheckBoxAppearance(PdfAnnotation* annot) {
		PiPiAppearanceUtil::GenerateCheckBoxDownCheckAppearance(annot);
		PiPiAppearanceUtil::GenerateCheckBoxDownUnCheckAppearance(annot);
		PiPiAppearanceUtil::GenerateCheckBoxNormalCheckAppearance(annot);
	}

	void PiPiAppearanceUtil::GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot) {
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

	void PiPiAppearanceUtil::GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot) {
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

	void PiPiAppearanceUtil::GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot) {
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

	void PiPiAppearanceUtil::GenerateTextBoxAppearance(PdfAnnotation* annot) {
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

		painter->FinishDrawing();

		delete painter;

		annot->SetAppearanceStream(xObjectRef);
	}
}