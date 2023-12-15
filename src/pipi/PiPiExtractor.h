#pragma once

#include "podofo/podofo.h"
#include "PiPiPage.h"
#include "PiPiField.h"
#include "PiPiAnnotationObserver.h"
#include "PiPiFieldUtil.h"
#include "PiPiExtractUtil.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiExtractor {
		public:
			PiPiExtractor(PdfMemDocument* document, PiPiAnnotationObserver* annotObserver);

			std::vector<const PiPiPage*>* extractPage();
			std::vector<const PiPiField*>* extractField();

			bool isOperable();

		private:
			PdfMemDocument* document;
            PiPiAnnotationObserver* annotObserver;
	};
}
