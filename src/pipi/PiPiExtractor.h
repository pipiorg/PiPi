#pragma once

#include "podofo/podofo.h"
#include "PiPiPage.h"
#include "PiPiField.h"
#include "PiPiPageUtil.h"
#include "PiPiFieldManager.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiExtractor {
		public:
			PiPiExtractor(PdfMemDocument* document, PiPiFieldManager* fieldManager);

			std::vector<const PiPiPage*>* ExtractPage();
			std::vector<const PiPiField*>* ExtractField();

			bool IsOperable();

		private:
			PdfMemDocument* document;
            PiPiFieldManager* fieldManager;
	};
}
