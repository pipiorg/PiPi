#pragma once

#include <string>
#include "PiPiFieldType.h"
#include "PiPiTextHorizontalAlignment.h"

namespace PiPi
{
	class PiPiField
	{
	public:
		PiPiField(
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
			float borderColorBlue);

		std::string name;

		PiPiFieldType type;
		PiPiTextHorizontalAlignment textHorizontalAlignment;

		unsigned int pageIndex;

		double x;
		double y;

		double width;
		double height;

		std::string fontName;
		float fontSize;

		bool backgroundExists;
		float backgroundColorRed;
		float backgroundColorGreen;
		float backgroundColorBlue;

		bool borderExists;
		double borderWidth;
		float borderColorRed;
		float borderColorGreen;
		float borderColorBlue;

		float colorRed;
		float colorGreen;
		float colorBlue;

		bool multiline;

		std::string defaultValue;
	};
}
