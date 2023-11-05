#pragma once

#include "podofo/podofo.h"
#include "PiPiPage.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiExtractor {
		public:
			PiPiExtractor(PdfMemDocument* document);

			std::vector<const PiPiPage*>* extractPage();

		private:
			PdfMemDocument* document;

			void init(PdfMemDocument* document);
	};
}