#include "PiPiPager.h"

namespace PiPi {
	PiPiPager::PiPiPager(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiPager::init(PdfMemDocument* document) {
		this->document = document;
	}

	bool PiPiPager::isOperable() {
		return this->document != nullptr;
	}
}