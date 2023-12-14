#pragma once

#include <iostream>
#include "podofo/podofo.h"
#include "PiPiUtil.h"
#include "PiPiAppearanceUtil.h"
#include "PiPiCommon.h"
#include "PiPiFontManager.h"
#include "PiPiTextHorizontalAlignment.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiEditor {
		public:
			PiPiEditor(PdfMemDocument* document, PiPiFontManager* fontManager);

            PiPiEditor* flatten();
			PiPiEditor* flatten(std::string fieldName);
        
            PiPiEditor* addField(std::string fieldName, PiPiFieldType type, unsigned int page, double x, double y, double width, double height);
			PiPiEditor* removeField(std::string fieldName);
			PiPiEditor* renameField(std::string oldFieldName, std::string newFieldName);

            PiPiEditor* setFieldFontName(std::string fieldName, std::string fontName);
            PiPiEditor* setFieldFontSize(std::string fieldName, float fontSize);
            
            PiPiEditor* setFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment);
        
            PiPiEditor* setFieldColor(std::string fieldName, int red, int green, int blue);
            PiPiEditor* setFieldBackgroundColor(std::string fieldName, int red, int green, int blue);
            PiPiEditor* setFieldBorderColor(std::string fieldName, int red, int green, int blue);
        
            PiPiEditor* setFieldMultiline(std::string fieldName, bool multiline);
        
			bool isOperable();

		private:
			void init(PdfMemDocument* document, PiPiFontManager* fontManager);

			PdfMemDocument* document;
            PiPiFontManager* fontManager;
	};
}
