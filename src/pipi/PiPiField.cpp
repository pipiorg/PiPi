#include "PiPiField.h"

namespace PiPi
{
	PiPiField::PiPiField(
		std::string name,
		PiPiFieldType type,
		unsigned int pageIndex,
		double x,
		double y,
		double width,
		double height,
		std::string defaultValue,
		PiPiTextHorizontalAlignment textHorizontalAlignment,
		std::string fontName,
		float fontSize,
		float colorRed,
		float colorGreen,
		float colorBlue,
		bool multiline,
		bool backgroundExists,
		float backgroundColorRed,
		float backgroundColorGreen,
		float backgroundColorBlue,
		bool borderExists,
		double borderWidth,
		float borderColorRed,
		float borderColorGreen,
		float borderColorBlue)
	{
		this->name = name;
		this->type = type;
		this->pageIndex = pageIndex;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->defaultValue = defaultValue;
		this->textHorizontalAlignment = textHorizontalAlignment;
		this->fontName = fontName;
		this->fontSize = fontSize;
		this->colorRed = colorRed;
		this->colorGreen = colorGreen;
		this->colorBlue = colorBlue;
		this->multiline = multiline;
		this->backgroundExists = backgroundExists;
		this->backgroundColorRed = backgroundColorRed;
		this->backgroundColorGreen = backgroundColorGreen;
		this->backgroundColorBlue = backgroundColorBlue;
		this->borderExists = borderExists;
		this->borderWidth = borderWidth;
		this->borderColorRed = borderColorRed;
		this->borderColorGreen = borderColorGreen;
		this->borderColorBlue = borderColorBlue;
		
	}
}
