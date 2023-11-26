#pragma once

#include <podofo/podofo.h>

using namespace PoDoFo;

namespace PiPi {
	class PiPiFontManager {
		public:
			PiPiFontManager(PdfMemDocument* document);

			void registerFont(char* fontBytes, size_t fontSize);

		private:
			PdfMemDocument* document;

			void init(PdfMemDocument* document);
	};
}