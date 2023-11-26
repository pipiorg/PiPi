#pragma once

#include "podofo/podofo.h"
#include "PiPiUtil.h"
#include "PiPiAppearanceUtil.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiFiller {
		public:
			PiPiFiller(PdfMemDocument* document);

			PiPiFiller* fillValue(std::string fieldName, std::string value);
			PiPiFiller* fillImage(std::string fieldName, char* imageBytes, size_t imageSize);

			bool isOperable();

		private:
			PdfMemDocument* document;

			void init(PdfMemDocument* document);
	};
}