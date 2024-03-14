#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include <utf8.h>
#include <unicode/unistr.h>
#include <unicode/normlzr.h>
#include "PiPiFontManager.h"
#include "PiPiAppearanceManager.h"
#include "PiPiFieldManager.h"
#include "PiPiFillFieldException.h"
#include "PiPiFlattenManager.h"

using namespace PoDoFo;

namespace PiPi
{
	class PiPiOperator;

	class PiPiFiller
	{
		friend class PiPiOperator;

	public:
		PiPiFiller(PdfMemDocument *document, PiPiFontManager *fontManager, PiPiAppearanceManager *appearanceManager, PiPiFieldManager *fieldManager, PiPiFlattenManager *flattenManager);

		PiPiFiller *FillValue(std::string fieldName, std::string value);
		PiPiFiller *FillValue(std::string fieldName, std::string value, bool ellipsis);
		PiPiFiller *FillImage(std::string fieldName, char *imageBytes, size_t imageSize);

		bool IsOperable();

	private:
		bool operable;

		PdfMemDocument *document;

		PiPiFontManager *fontManager;
		PiPiAppearanceManager *appearanceManager;
		PiPiFieldManager *fieldManager;
		PiPiFlattenManager *flattenManager;

		void DirectFillValue(std::string fieldName, std::string value);

		std::string FilterValue(std::string value, std::string fontName);
		std::string TrimValue(std::string value, double width, std::string fontName, float fontSize);
		std::string NormalizeValue(std::string value);
		std::string EllipsisValue(std::string value, double width, double height, std::string fontName, float fontSize);
		std::string EllipsisValueMultiline(std::string value, double width, double height, std::string fontName, float fontSize);
	};
}
