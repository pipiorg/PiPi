#pragma once

#include <iostream>
#include "podofo/podofo.h"
#include "PiPiUtil.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiEditor {
		public:
			PiPiEditor(PdfMemDocument* document);

            PiPiEditor* flatten();
			PiPiEditor* flatten(std::string fieldName);
			PiPiEditor* removeFormField(std::string fieldName);
			PiPiEditor* renameFormField(std::string oldFieldName, std::string newFieldName);

			bool isOperable();

		private:
			void init(PdfMemDocument* document);

			PdfMemDocument* document;
	};
}
