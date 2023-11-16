#pragma once

#include "podofo/podofo.h"
#include "PiPiPage.h"
#include "PiPiField.h"
#include "PiPiUtil.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiExtractor {
		public:
			PiPiExtractor(PdfMemDocument* document);

			std::vector<const PiPiPage*>* extractPage();
			std::vector<const PiPiField*>* extractField();

			bool isOperable();

		private:
			PdfMemDocument* document;

			void init(PdfMemDocument* document);
	};
}