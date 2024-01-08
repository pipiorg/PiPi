#pragma once

#include "podofo/podofo.h"
#include "utf8.h"
#include "PiPiFontManager.h"
#include "PiPiAppearanceManager.h"
#include "PiPiFieldManager.h"
#include "PiPiFillFieldException.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiOperator;

	class PiPiFiller {
		friend class PiPiOperator;

		public:
			PiPiFiller(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAppearanceManager* appearanceManager, PiPiFieldManager* fieldManager);

			PiPiFiller* FillValue(std::string fieldName, std::string value);
            PiPiFiller* FillValue(std::string fieldName, std::string value, bool ellipsis);
			PiPiFiller* FillImage(std::string fieldName, char* imageBytes, size_t imageSize);

			bool IsOperable();

		private:
			bool operable;

			PdfMemDocument* document;
			
			PiPiFontManager* fontManager;
			PiPiAppearanceManager* appearanceManager;
            PiPiFieldManager* fieldManager;
        
            void DirectFillValue(std::string fieldName, std::string value);

            std::string FilterValue(std::string value, std::string fontName);
            std::string TrimValue(std::string value, float width, std::string fontName, float fontSize);
            std::string EllipsisValue(std::string value, float width, float height, std::string fontName, float fontSize);
            std::string EllipsisValueMultiline(std::string value, float width, float height, std::string fontName, float fontSize);
	};
}
