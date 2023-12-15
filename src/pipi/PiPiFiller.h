#pragma once

#include "podofo/podofo.h"
#include "utf8.h"
#include "PiPiFontManager.h"
#include "PiPiFieldUtil.h"
#include "PiPiAppearanceUtil.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiFiller {
		public:
			PiPiFiller(PdfMemDocument* document, PiPiFontManager* fontManager);

			PiPiFiller* fillValue(std::string fieldName, std::string value);
            PiPiFiller* fillValue(std::string fieldName, std::string value, bool ellipsis);
			PiPiFiller* fillImage(std::string fieldName, char* imageBytes, size_t imageSize);

			bool isOperable();

		private:
			PdfMemDocument* document;
			
			PiPiFontManager* fontManager;

			void init(PdfMemDocument* document, PiPiFontManager* fontManager);
            std::string ellipsisValue(std::string value, float width, float height, std::string fontName, float fontSize);
            std::string ellipsisValueMultiline(std::string value, float width, float height, std::string fontName, float fontSize);
	};
}
