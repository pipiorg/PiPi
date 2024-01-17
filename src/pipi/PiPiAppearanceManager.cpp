#include "PiPiAppearanceManager.h"

namespace PiPi
{
	PiPiAppearanceManager::PiPiAppearanceManager(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiFieldManager* fieldManager)
	{
		this->document = document;
		this->fontManager = fontManager;
		this->fieldManager = fieldManager;

		this->fieldNames = new std::set<std::string>();
	}

	PiPiAppearanceManager::~PiPiAppearanceManager()
	{
		if (this->fieldNames)
		{
			delete this->fieldNames;
			this->fieldNames = nullptr;
		}
	}

	void PiPiAppearanceManager::GenerateAppearance()
	{
		spdlog::trace("GenerateAppearance");

		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = this->fontManager;
		PiPiFieldManager* fieldManager = this->fieldManager;
		std::set<std::string>* fieldNames = this->fieldNames;

		std::map<const std::string, std::set<PdfField*>*>* fieldMap = fieldManager->SearchAllField();
		for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++())
		{
			std::set<PdfField*>* fields = mapIterator->second;

			for (auto fieldIterator = fields->begin(); fieldIterator != fields->end(); fieldIterator.operator++())
			{
				PdfField* field = *fieldIterator;
				PdfAnnotation* annot = fieldManager->BridgeFieldToAnnotation(field);

				PdfFieldType type = field->GetType();
				std::string fieldName = field->GetFullName();

				if (type == PdfFieldType::CheckBox || fieldNames->find(fieldName) != fieldNames->end())
				{
					this->GenerateAppearance(fontManager, fieldManager, annot);
				}
			}
		}

		this->ClearNeedAppearance();
	}

	void PiPiAppearanceManager::GenerateAppearance(std::string name)
	{
		spdlog::trace("GenerateAppearance");

		PdfMemDocument* document = this->document;
		PiPiFontManager* fontManager = this->fontManager;
		PiPiFieldManager* fieldManager = this->fieldManager;
		std::set<std::string>* fieldNames = this->fieldNames;

		std::map<const std::string, std::set<PdfField*>*>* fieldMap = fieldManager->SearchAllField();
		for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++())
		{
			std::set<PdfField*>* fields = mapIterator->second;

			for (auto fieldIterator = fields->begin(); fieldIterator != fields->end(); fieldIterator.operator++())
			{
				PdfField* field = *fieldIterator;
				PdfAnnotation* annot = fieldManager->BridgeFieldToAnnotation(field);

				std::string fieldName = field->GetFullName();

				if (fieldName == name && fieldNames->find(fieldName) != fieldNames->end())
				{
					this->GenerateAppearance(fontManager, fieldManager, annot);
					this->UnMarkNeedAppearance(fieldName);
				}
			}
		}
	}

	void PiPiAppearanceManager::MarkNeedAppearance(std::string fieldName)
	{
		spdlog::trace("MarkNeedAppearance");

		std::set<std::string>* fieldNames = this->fieldNames;
		fieldNames->insert(fieldName);
	}

	void PiPiAppearanceManager::UnMarkNeedAppearance(std::string fieldName)
	{
		spdlog::trace("UnMarkNeedAppearance");

		std::set<std::string>* fieldNames = this->fieldNames;
		fieldNames->erase(fieldName);
	}

	void PiPiAppearanceManager::ClearNeedAppearance()
	{
		spdlog::trace("ClearNeedAppearance");

		std::set<std::string>* fieldNames = this->fieldNames;
		fieldNames->clear();
	}

	void PiPiAppearanceManager::GenerateAppearance(PiPiFontManager* fontManager, PiPiFieldManager* fieldManager, PdfAnnotation* annot)
	{
		spdlog::trace("GenerateAppearance");

		PiPiFieldType type = PiPiAnnotationUtil::ExtractAnnotationType(annot);

		switch (type)
		{
		case PiPiFieldType::TextBox:
			this->GenerateTextBoxAppearance(fontManager, annot);
			break;
		case PiPiFieldType::CheckBox:
			this->ClearCheckBoxAppearance(annot);
			this->GenerateCheckBoxAppearance(annot);
			break;
		case PiPiFieldType::Unknown:
		default:
			throw PiPiAppearanceException(PiPiAppearanceException::PiPiAppearanceExceptionCode::UnsupportedFieldType);
		}
	}

	void PiPiAppearanceManager::ClearCheckBoxAppearance(PdfAnnotation* annot)
	{
		spdlog::trace("ClearCheckBoxAppearance");

		PdfDictionary* dict = &(annot->GetDictionary());

		PdfObject* apObj = dict->FindKey(PdfName("AP"));
		if (apObj == nullptr)
		{
			return;
		}

		PdfDictionary* ap = &(apObj->GetDictionary());

		PdfObject* nObj = ap->FindKey(PdfName("N"));
		if (nObj != nullptr)
		{
			PdfDictionary* n = &(nObj->GetDictionary());
			n->Clear();
		}

		PdfObject* dObj = ap->FindKey(PdfName("D"));
		if (nObj != nullptr)
		{
			PdfDictionary* d = &(dObj->GetDictionary());
			d->Clear();
		}
	}

	void PiPiAppearanceManager::GenerateCheckBoxAppearance(PdfAnnotation* annot)
	{
		spdlog::trace("GenerateCheckBoxAppearance");

		this->GenerateCheckBoxDownCheckAppearance(annot);
		this->GenerateCheckBoxDownUnCheckAppearance(annot);
		this->GenerateCheckBoxNormalCheckAppearance(annot);
	}

	void PiPiAppearanceManager::GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot)
	{
		spdlog::trace("GenerateCheckBoxNormalCheckAppearance");

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

		bool borderExists = PiPiAnnotationUtil::ExtractAnnotationBorderExists(annot);
		if (borderExists)
		{
			painter->Save();

			float borderColorRed = 0;
			float borderColorGreen = 0;
			float borderColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);

			borderWidth = PiPiAnnotationUtil::ExtractAnnotationBorderWidth(annot);

			graphicsState->SetLineWidth(borderWidth);
			graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
			graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);

			painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);

			painter->Restore();
		}

		// 畫背景
		bool backgroundExists = PiPiAnnotationUtil::ExtractAnnotationBackgroundExists(annot);
		if (backgroundExists)
		{
			painter->Save();

			float backgroundColorRed = 0;
			float backgroundColorGreen = 0;
			float backgroundColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);

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
		PiPiAnnotationUtil::ExtractAnnotationColor(annot, &colorRed, &colorGreen, &colorBlue);

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

	void PiPiAppearanceManager::GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot)
	{
		spdlog::trace("GenerateCheckBoxDownCheckAppearance");

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

		bool borderExists = PiPiAnnotationUtil::ExtractAnnotationBorderExists(annot);
		if (borderExists)
		{
			painter->Save();

			borderWidth = PiPiAnnotationUtil::ExtractAnnotationBorderWidth(annot);

			float borderColorRed = 0;
			float borderColorGreen = 0;
			float borderColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);

			graphicsState->SetLineWidth(borderWidth);
			graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
			graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);

			painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);

			painter->Restore();
		}

		// 畫背景
		bool backgroundExists = PiPiAnnotationUtil::ExtractAnnotationBackgroundExists(annot);
		if (backgroundExists)
		{
			painter->Save();

			float backgroundColorRed = 0;
			float backgroundColorGreen = 0;
			float backgroundColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);

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
		PiPiAnnotationUtil::ExtractAnnotationColor(annot, &colorRed, &colorGreen, &colorBlue);

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

	void PiPiAppearanceManager::GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot)
	{
		spdlog::trace("GenerateCheckBoxDownUnCheckAppearance");

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

		bool borderExists = PiPiAnnotationUtil::ExtractAnnotationBorderExists(annot);
		if (borderExists)
		{
			painter->Save();

			borderWidth = PiPiAnnotationUtil::ExtractAnnotationBorderWidth(annot);

			float borderColorRed = 0;
			float borderColorGreen = 0;
			float borderColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);

			graphicsState->SetLineWidth(borderWidth);
			graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
			graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);

			painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);

			painter->Restore();
		}

		// 畫背景
		bool backgroundExists = PiPiAnnotationUtil::ExtractAnnotationBackgroundExists(annot);
		if (backgroundExists)
		{
			painter->Save();

			float backgroundColorRed = 0;
			float backgroundColorGreen = 0;
			float backgroundColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);

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

	void PiPiAppearanceManager::GenerateTextBoxAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot)
	{
		spdlog::trace("GenerateTextBoxAppearance");

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

		bool borderExists = PiPiAnnotationUtil::ExtractAnnotationBorderExists(annot);
		if (borderExists)
		{
			painter->Save();

			borderWidth = PiPiAnnotationUtil::ExtractAnnotationBorderWidth(annot);

			float borderColorRed = 0;
			float borderColorGreen = 0;
			float borderColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBorderColor(annot, &borderColorRed, &borderColorGreen, &borderColorBlue);

			graphicsState->SetLineWidth(borderWidth);
			graphicsState->SetStrokeColor(PdfColor(borderColorRed, borderColorGreen, borderColorBlue));
			graphicsState->SetLineCapStyle(PdfLineCapStyle::Round);

			painter->DrawRectangle(borderWidth / 2, borderWidth / 2, width - borderWidth, height - borderWidth, PdfPathDrawMode::Stroke);

			painter->Restore();
		}

		// 畫背景
		bool backgroundExists = PiPiAnnotationUtil::ExtractAnnotationBackgroundExists(annot);
		if (backgroundExists)
		{
			painter->Save();

			float backgroundColorRed = 0;
			float backgroundColorGreen = 0;
			float backgroundColorBlue = 0;
			PiPiAnnotationUtil::ExtractAnnotationBackgroundColor(annot, &backgroundColorRed, &backgroundColorGreen, &backgroundColorBlue);

			graphicsState->SetFillColor(PdfColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue));

			painter->DrawRectangle(borderWidth, borderWidth, width - borderWidth * 2, height - borderWidth * 2, PdfPathDrawMode::Fill);

			painter->Restore();
		}

		// 畫文字
		painter->Save();

		float textColorRed = 0;
		float textColorGreen = 0;
		float textColorBlue = 0;
		PiPiAnnotationUtil::ExtractAnnotationColor(annot, &textColorRed, &textColorGreen, &textColorBlue);

		graphicsState->SetFillColor(PdfColor(textColorRed, textColorGreen, textColorBlue));
		graphicsState->SetStrokeColor(PdfColor(textColorRed, textColorGreen, textColorBlue));

		float fontSize = PiPiAnnotationUtil::ExtractAnnotationFontSize(annot);
		std::string fontName = PiPiAnnotationUtil::ExtractAnnotationFontName(annot);

		const PdfFont* font = fontManager->AccessFont(fontName) == nullptr
			? fontManager->AccessDefaultFont()
			: fontManager->AccessFont(fontName);
		const PdfFont& fontRef = *font;

		textState->SetFont(fontRef, fontSize);

		std::string sText = PiPiAnnotationUtil::ExtractAnnotationValue(annot);

		// 為了垂直置中，不管多行單行都照多行套
		// 前面套印處以過濾掉會變多行的文字
		PdfDrawTextMultiLineParams multineParams;

		bool multiline = PiPiAnnotationUtil::ExtractAnnotationTextMultiine(annot);
		multineParams.VerticalAlignment = multiline
			? PdfVerticalAlignment::Top
			: PdfVerticalAlignment::Center;

		PiPiTextHorizontalAlignment horizontalAlignment = PiPiAnnotationUtil::ExtractAnnotationTextHorizontalAlignment(annot);
		switch (horizontalAlignment)
		{
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
