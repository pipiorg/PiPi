#pragma once

#include <iostream>
#include "podofo/podofo.h"
#include "PiPiUtil.h"
#include "PiPiCommon.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiEditor {
		public:
			PiPiEditor(PdfMemDocument* document);

            PiPiEditor* flatten();
			PiPiEditor* flatten(std::string fieldName);
            PiPiEditor* addField(std::string fieldName, PiPiFieldType type, unsigned int page, double x, double y, double width, double height);
			PiPiEditor* removeField(std::string fieldName);
			PiPiEditor* renameField(std::string oldFieldName, std::string newFieldName);

			bool isOperable();

		private:
			void init(PdfMemDocument* document);

			PdfMemDocument* document;
	};
}
