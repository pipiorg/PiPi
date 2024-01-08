#pragma once

#include "podofo/podofo.h"
#include "PiPiPage.h"
#include "PiPiField.h"
#include "PiPiPageUtil.h"
#include "PiPiFieldManager.h"
#include "PiPiExtractException.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiOperator;

	class PiPiExtractor {
		friend class PiPiOperator;

		public:
			PiPiExtractor(PdfMemDocument* document, PiPiFieldManager* fieldManager);

			std::vector<const PiPiPage*>* ExtractPage();
			std::vector<const PiPiField*>* ExtractField();

			bool IsOperable();

		private:
			bool operable;

			PdfMemDocument* document;
            PiPiFieldManager* fieldManager;
	};
}
