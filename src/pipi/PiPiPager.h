#pragma once

#include "podofo/podofo.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiPager {
		public:
			PiPiPager(PdfMemDocument* document);

			bool IsOperable();

		private:
			PdfMemDocument* document;
	};
}
