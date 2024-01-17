#include "PiPiFiller.h"

namespace PiPi
{
	PiPiFiller::PiPiFiller(PdfMemDocument *document, PiPiFontManager *fontManager, PiPiAppearanceManager *appearanceManager, PiPiFieldManager *fieldManager, PiPiFlattenManager *flattenManager)
	{
		this->operable = true;
		this->document = document;
		this->appearanceManager = appearanceManager;
		this->fontManager = fontManager;
		this->fieldManager = fieldManager;
		this->flattenManager = flattenManager;
	}

	bool PiPiFiller::IsOperable()
	{
		return this->operable;
	}

	PiPiFiller *PiPiFiller::FillValue(std::string name, std::string value)
	{
		spdlog::trace("FillValue");

		if (!this->IsOperable())
		{
			throw PiPiFillFieldException(PiPiFillFieldException::PiPiFillFieldExceptionCode::InOperable);
		}

		PdfMemDocument *document = this->document;
		PiPiFieldManager *fieldManager = this->fieldManager;

		std::set<PdfField *> *fields = fieldManager->SearchField(name);

		double minArea = -1;
		PdfAnnotation *minAnnot = nullptr;

		for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
		{
			PdfField *field = *iterator;
			PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

			double width = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
			double height = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);
			double area = width * height;

			if (minArea == -1 || area < minArea)
			{
				minAnnot = annot;
				minArea = area;
				continue;
			}
		}

		delete fields;

		if (minAnnot == nullptr)
		{
			return this;
		}

		bool minMultiline = PiPiAnnotationUtil::ExtractAnnotationTextMultiine(minAnnot);

		std::string minFontName = PiPiAnnotationUtil::ExtractAnnotationFontName(minAnnot);
		float minFontSize = PiPiAnnotationUtil::ExtractAnnotationFontSize(minAnnot);

		double minWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(minAnnot);

		PiPiFieldType minType = PiPiAnnotationUtil::ExtractAnnotationType(minAnnot);

		value = minType == PiPiFieldType::TextBox
								? this->FilterValue(value, minFontName)
								: value;

		value = minType == PiPiFieldType::TextBox && !minMultiline
								? this->TrimValue(value, minWidth, minFontName, minFontSize)
								: value;

		this->DirectFillValue(name, value);

		return this;
	}

	PiPiFiller *PiPiFiller::FillValue(std::string fieldName, std::string value, bool ellipsis)
	{
		spdlog::trace("FillValue");

		if (!this->IsOperable())
		{
			throw PiPiFillFieldException(PiPiFillFieldException::PiPiFillFieldExceptionCode::InOperable);
		}

		PdfMemDocument *document = this->document;
		PiPiFieldManager *fieldManager = this->fieldManager;

		PdfAnnotation *minAnnot = nullptr;
		double minArea = -1;

		std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);
		for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
		{
			PdfField *field = *iterator;
			PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

			double width = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
			double height = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);
			double area = width * height;

			if (minArea == -1 || area < minArea)
			{
				minAnnot = annot;
				minArea = area;
				continue;
			}
		}

		delete fields;

		if (minAnnot == nullptr)
		{
			return this;
		}

		PiPiFieldType minType = PiPiAnnotationUtil::ExtractAnnotationType(minAnnot);

		std::string minFontName = PiPiAnnotationUtil::ExtractAnnotationFontName(minAnnot);

		value = this->FilterValue(value, minFontName);

		if (ellipsis)
		{
			if (minType == PiPiFieldType::TextBox)
			{
				bool minMultiline = PiPiAnnotationUtil::ExtractAnnotationTextMultiine(minAnnot);

				float minFontSize = PiPiAnnotationUtil::ExtractAnnotationFontSize(minAnnot);

				double minWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(minAnnot);
				double minHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(minAnnot);

				value = minMultiline
										? this->EllipsisValueMultiline(value, minWidth, minHeight, minFontName, minFontSize)
										: this->EllipsisValue(value, minWidth, minHeight, minFontName, minFontSize);
			}
		}

		this->DirectFillValue(fieldName, value);

		return this;
	}

	PiPiFiller *PiPiFiller::FillImage(std::string fieldName, char *imageBytes, size_t imageSize)
	{
		spdlog::trace("FillImage");

		if (!this->IsOperable())
		{
			throw PiPiFillFieldException(PiPiFillFieldException::PiPiFillFieldExceptionCode::InOperable);
		}

		PdfMemDocument *document = this->document;
		PiPiFieldManager *fieldManager = this->fieldManager;
		PiPiFlattenManager *flattenManager = this->flattenManager;
		PiPiAppearanceManager *appearanceManager = this->appearanceManager;

		std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);
		for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
		{
			PdfField *field = *iterator;
			PdfAnnotation *annot = fieldManager->BridgeFieldToAnnotation(field);

			std::unique_ptr<PdfImage> imageUniquePtr = document->CreateImage();
			PdfImage *image = imageUniquePtr.get();
			image->LoadFromBuffer(bufferview(imageBytes, imageSize));

			flattenManager->FlattenImage(annot, image);
		}

		delete fields;

		fieldManager->RemoveField(fieldName);
		appearanceManager->UnMarkNeedAppearance(fieldName);

		return this;
	}

	void PiPiFiller::DirectFillValue(std::string fieldName, std::string value)
	{
		spdlog::trace("DirectFillValue");

		PiPiFieldManager *fieldManager = this->fieldManager;
		PiPiAppearanceManager *appearanceManager = this->appearanceManager;

		std::set<PdfField *> *fields = fieldManager->SearchField(fieldName);
		for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
		{
			PdfField *field = *iterator;

			PdfFieldType type = field->GetType();

			if (type == PdfFieldType::TextBox)
			{
				PdfTextBox *textBoxField = (PdfTextBox *)field;

				PdfString valueString(value);
				textBoxField->SetText(valueString);
			}
			else if (type == PdfFieldType::CheckBox)
			{
				PdfCheckBox *checkBoxField = (PdfCheckBox *)field;

				bool checked = value == "Yes" || value == "On";
				checkBoxField->SetChecked(checked);
			}
			else
			{
				throw PiPiFillFieldException(PiPiFillFieldException::PiPiFillFieldExceptionCode::UnsupportedPdfFieldType);
			}
		}

		delete fields;

		appearanceManager->MarkNeedAppearance(fieldName);
	}

	std::string PiPiFiller::FilterValue(std::string value, std::string fontName)
	{
		spdlog::trace("FilterValue");

		PiPiFontManager *fontManager = this->fontManager;

		const PdfFont *font = fontManager->AccessFont(fontName) == nullptr
															? fontManager->AccessDefaultFont()
															: fontManager->AccessFont(fontName);

		const PdfEncoding *encoding = &(font->GetEncoding());

		std::string replaceValue = " ";
		charbuff replaceValueEncoded;
		bool replaceConverted = encoding->TryConvertToEncoded(replaceValue, replaceValueEncoded);
		if (!replaceConverted)
		{
			replaceValue = "";
		}

		std::string newValue = "";

		auto iterator = value.begin();
		while (iterator != value.end())
		{
			utf8::utfchar32_t character = utf8::next(iterator, value.end());

			std::string characterString = "";
			utf8::append(character, characterString);

			charbuff encoded;
			bool converted = encoding->TryConvertToEncoded(characterString, encoded);

			newValue += converted ? characterString : replaceValue;
		}

		return newValue;
	}

	std::string PiPiFiller::TrimValue(std::string value, double width, std::string fontName, float fontSize)
	{
		spdlog::trace("TrimValue");

		PiPiFontManager *fontManager = this->fontManager;

		const PdfFont *font = fontManager->AccessFont(fontName) == nullptr
															? fontManager->AccessDefaultFont()
															: fontManager->AccessFont(fontName);

		PdfTextState textState;
		textState.Font = font;
		textState.FontSize = fontSize;
		textState.FontScale = 1.0;
		textState.RenderingMode = PdfTextRenderingMode::Fill;

		double lineWidth = font->GetStringLength(value, textState);
		if (lineWidth < width)
		{
			return value;
		}

		std::string line = "";
		std::string nextLine = "";

		auto iterator = value.begin();
		while (iterator != value.end())
		{
			utf8::utfchar32_t character = utf8::next(iterator, value.end());

			utf8::append(character, nextLine);

			double lineWidth = font->GetStringLength(nextLine, textState);
			if (lineWidth > width)
			{
				break;
			}

			utf8::append(character, line);
		}

		return line;
	}

	std::string PiPiFiller::EllipsisValue(std::string value, double width, double height, std::string fontName, float fontSize)
	{
		spdlog::trace("EllipsisValue");

		PiPiFontManager *fontManager = this->fontManager;

		const PdfFont *font = fontManager->AccessFont(fontName) == nullptr
															? fontManager->AccessDefaultFont()
															: fontManager->AccessFont(fontName);

		PdfTextState textState;
		textState.Font = font;
		textState.FontSize = fontSize;
		textState.FontScale = 1.0;
		textState.RenderingMode = PdfTextRenderingMode::Fill;

		double fLineWidth = font->GetStringLength(value, textState);
		if (fLineWidth < width)
		{
			return value;
		}

		std::string line = "";
		std::string nextLine = "";

		auto iterator = value.begin();
		while (iterator != value.end())
		{
			utf8::utfchar32_t character = utf8::next(iterator, value.end());

			utf8::append(character, nextLine);

			std::string nextLineWithEllipsis = nextLine + "...";

			double lineWidth = font->GetStringLength(nextLineWithEllipsis, textState);
			if (lineWidth > width)
			{
				line += "...";
				break;
			}

			utf8::append(character, line);
		}

		return line;
	}

	std::string PiPiFiller::EllipsisValueMultiline(std::string value, double width, double height, std::string fontName, float fontSize)
	{
		spdlog::trace("EllipsisValueMultiline");

		PiPiFontManager *fontManager = this->fontManager;

		const PdfFont *font = fontManager->AccessFont(fontName) == nullptr
															? fontManager->AccessDefaultFont()
															: fontManager->AccessFont(fontName);

		PdfTextState textState;
		textState.Font = font;
		textState.FontSize = fontSize;
		textState.FontScale = 1.0;
		textState.RenderingMode = PdfTextRenderingMode::Fill;

		// 切割每行有哪些字
		std::vector<std::string> *lines = new std::vector<std::string>();

		std::string line = "";
		std::string nextLine = "";

		auto iterator = value.begin();
		while (iterator != value.end())
		{
			utf8::utfchar32_t character = utf8::next(iterator, value.end());

			utf8::append(character, nextLine);

			double lineWidth = font->GetStringLength(nextLine, textState);
			if (lineWidth > width)
			{
				lines->push_back(line);

				line = "";
				nextLine = "";

				utf8::append(character, line);
				utf8::append(character, nextLine);

				continue;
			}

			utf8::append(character, line);
		}

		if (line.length() != 0)
		{
			lines->push_back(line);
		}

		long lineCount = lines->size();
		if (lineCount == 0)
		{
			return "";
		}

		// 計算可塞下幾行
		double lineSpacing = font->GetLineSpacing(textState);
		double ascent = font->GetAscent(textState);
		double descent = font->GetDescent(textState);

		double lineGap = lineSpacing - ascent + descent;
		double lineHeight = lineSpacing;

		double totalLineHieght = -(lineHeight - ascent + lineGap / 2);

		long availableLineCount = 0;
		for (long lineIndex = 0; lineIndex < lineCount; lineIndex++)
		{
			if (totalLineHieght + lineHeight > height)
			{
				break;
			}

			totalLineHieght += lineHeight;
			availableLineCount++;
		}

		if (availableLineCount == 0)
		{
			return "";
		}

		// 開始湊完整值
		value = "";
		for (long i = 0; i < availableLineCount - 1; i++)
		{
			value += (*lines)[i];
		}

		// 開始判斷最後一行省略狀態
		std::string lastLine = (*lines)[availableLineCount - 1];

		bool shouldEllipsis = lineCount > availableLineCount;
		if (shouldEllipsis)
		{
			std::string newLastLine = "";
			std::string postNewLastLine = "";

			bool added = false;
			auto iterator = lastLine.begin();
			while (iterator != lastLine.end())
			{
				utf8::utfchar32_t character = utf8::next(iterator, lastLine.end());

				utf8::append(character, postNewLastLine);

				std::string postNewLastLineWithEllipsis = postNewLastLine + "...";

				double postNewLastLineWithEllipsisWidth = font->GetStringLength(postNewLastLineWithEllipsis, textState);

				if (postNewLastLineWithEllipsisWidth > width)
				{
					added = true;
					newLastLine += "...";
					break;
				}

				utf8::append(character, newLastLine);
			}

			if (!added)
			{
				newLastLine += "...";
			}

			value += newLastLine;
		}
		else
		{
			value += lastLine;
		}

		delete lines;

		return value;
	}
}
