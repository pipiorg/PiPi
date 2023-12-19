#pragma once

#include <iostream>
#include "podofo/podofo.h"
#include "PiPiFieldUtil.h"
#include "PiPiAnnotationUtil.h"
#include "PiPiAppearanceManager.h"
#include "PiPiCommon.h"
#include "PiPiFontManager.h"
#include "PiPiFieldObserver.h"
#include "PiPiAnnotationObserver.h"
#include "PiPiTextHorizontalAlignment.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiEditor {
		public:
			PiPiEditor(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAppearanceManager* appearanceManager, PiPiFieldObserver* fieldObserver, PiPiAnnotationObserver* annotObserver);

            PiPiEditor* flatten();
			PiPiEditor* flatten(std::string fieldName);
        
            PiPiEditor* addField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
			
            PiPiEditor* removeField(std::string fieldName);
            PiPiEditor* removeField(std::string fieldName, unsigned int pageIndex, double x, double y, double width, double height);
        
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
			PdfMemDocument* document;
        
            PiPiFontManager* fontManager;
            PiPiAppearanceManager* appearanceManager;
            PiPiFieldObserver* fieldObserver;
            PiPiAnnotationObserver* annotObserver;
	};
}
