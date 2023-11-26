#pragma once

#include <podofo/podofo.h>

using namespace PoDoFo;

namespace PiPi {
	class PiPiFontManager {
		public:
			PiPiFontManager(PdfMemDocument* document);

			void registerFont(char* fontBytes, size_t fontSize);
			const PdfFont* accessFont(std::string fontName);

		private:
			PdfMemDocument* document;

			std::map<const std::string, const PdfFont*>* fontMap;

			void init(PdfMemDocument* document);
	};
}