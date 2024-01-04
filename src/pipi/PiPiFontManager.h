#pragma once

#include <podofo/podofo.h>

using namespace PoDoFo;

namespace PiPi {
	class PiPiFontManager {
		public:
			PiPiFontManager(PdfMemDocument* document);

			bool IsOperable();

            void EmbedFonts();
        
			const std::string RegisterFont(char* fontBytes, size_t fontSize);
        
			const PdfFont* AccessFont(const std::string fontName);
			const PdfFont* AccessDefaultFont();

		private:
			PdfMemDocument* document;

			std::map<const std::string, const PdfFont*>* fontMap;

			void Init(PdfMemDocument* document);
			void LoadAcroformFonts();
        
            void EmbedDocumentFonts();
            void EmbedAcroformFonts();
	};
}
