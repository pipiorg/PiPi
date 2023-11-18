#include "PiPiField.h"

namespace PiPi {
	PiPiField::PiPiField(std::string name, PiPiFieldType type, unsigned int page, double x, double y, double width, double height, std::string fontName, float fontSize) {
		this->name = name;
		this->type = type;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->fontName = fontName;
		this->fontSize = fontSize;
		this->page = page;
	}
}