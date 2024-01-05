#pragma once

#include <iostream>
#include "podofo/podofo.h"
#include "PiPiStringCommon.h"
#include "PiPiAppearanceManager.h"
#include "PiPiFontManager.h"
#include "PiPiFieldManager.h"
#include "PiPiEditFieldException.h"
#include "PiPiTextHorizontalAlignment.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiEditor {
		public:
			PiPiEditor(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAppearanceManager* appearanceManager, PiPiFieldManager* fieldManager);

            PiPiEditor* Flatten();
			PiPiEditor* Flatten(std::string fieldName);
        
            PiPiEditor* AddField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height);
			
            PiPiEditor* RemoveField(std::string fieldName);
            PiPiEditor* RemoveField(std::string fieldName, long pageIndex);
            PiPiEditor* RemoveField(std::string fieldName, long pageIndex, double x, double y);
            PiPiEditor* RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height);
        
			PiPiEditor* RenameField(std::string oldFieldName, std::string newFieldName);

            PiPiEditor* SetFieldFontName(std::string fieldName, std::string fontName);
            PiPiEditor* SetFieldFontSize(std::string fieldName, float fontSize);
            
            PiPiEditor* SetFieldTextHorizontalAlignment(std::string fieldName, PiPiTextHorizontalAlignment alignment);
        
            PiPiEditor* SetFieldColor(std::string fieldName, float red, float green, float blue);
            PiPiEditor* SetFieldBackgroundColor(std::string fieldName, float red, float green, float blue);
            PiPiEditor* SetFieldBorderColor(std::string fieldName, float red, float green, float blue);
        
            PiPiEditor* SetFieldMultiline(std::string fieldName, bool multiline);
        
			bool IsOperable();

		private:
			PdfMemDocument* document;
        
            PiPiFontManager* fontManager;
            PiPiAppearanceManager* appearanceManager;
            PiPiFieldManager* fieldManager;
        
            const PdfString* GetDefaultDA();
	};
}
