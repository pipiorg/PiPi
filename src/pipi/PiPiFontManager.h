#pragma once

#include <podofo/podofo.h>

using namespace PoDoFo;

namespace PiPi {
	class PiPiFontManager {
		public:
			PiPiFontManager(PdfMemDocument* document);

			bool isOperable();

            void embedFonts();
        
			const std::string registerFont(char* fontBytes, size_t fontSize);
        
			const PdfFont* accessFont(const std::string fontName);
			const PdfFont* accessDefaultFont();

		private:
			PdfMemDocument* document;

			std::map<const std::string, const PdfFont*>* fontMap;

			void init(PdfMemDocument* document);
			void loadAcroformFonts();
            void embedAcroformFonts();
	};
}
