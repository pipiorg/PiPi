#pragma once

#include <iostream>
#include "podofo/podofo.h"
#include "PiPiUtil.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiEditor {
		public:
			PiPiEditor(PdfMemDocument* document);

			PiPiEditor* flatten(std::string fieldName);
			PiPiEditor* removeFormField(std::string fieldName);
			PiPiEditor* renameFormField(std::string oldFieldName, std::string newFieldName);

		private:
			void init(PdfMemDocument* document);

			PdfMemDocument* document;
	};
}