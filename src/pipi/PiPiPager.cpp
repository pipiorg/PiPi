#include "PiPiPager.h"

namespace PiPi {
	PiPiPager::PiPiPager(PdfMemDocument* document) {
        this->document = document;
	}

	bool PiPiPager::IsOperable() {
		return this->document != nullptr;
	}
}
