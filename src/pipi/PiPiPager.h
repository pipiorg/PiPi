#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiPager {
		public:
			PiPiPager(PdfMemDocument* document);

		private:
			PdfMemDocument* document;

			void init(PdfMemDocument* document);
	};
}