#include <iostream>
#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiFiller {
		public:
			PiPiFiller(PdfMemDocument* document);

			PiPiFiller* fillValue(std::string fieldName, std::string value);
			PiPiFiller* fillImage(std::string fieldName, char* imageBytes);

		private:
			PdfMemDocument* document;

			void init(PdfMemDocument* document);
	};
}