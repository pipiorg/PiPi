#pragma once

#include <string>
#include "PiPiFieldType.h"

namespace PiPi {
	class PiPiField {
		public:
			PiPiField(std::string name, PiPiFieldType type, unsigned int page, double x, double y, double width, double height, std::string font, float fontSize);

			std::string name;

			PiPiFieldType type;

			unsigned int page;
			
			double x;
			double y;
			
			double width;
			double height;

			std::string fontName;
			float fontSize;
	};
}