#include "PiPiField.h"

namespace PiPi {
	PiPiField::PiPiField(std::string name, PiPiFieldType type, int page, double x, double y, double width, double height, std::string font, float fontSize) {
		this->name = name;
		this->type = type;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->font = font;
		this->fontSize = fontSize;
	}
}