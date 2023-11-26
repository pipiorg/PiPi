#include "PiPiFontManager.h"

namespace PiPi {
	PiPiFontManager::PiPiFontManager(PdfMemDocument* document) {
		this->init(document);
	}

	void PiPiFontManager::init(PdfMemDocument* document) {
		this->document = document;
	}

	void PiPiFontManager::registerFont(char* fontBytes, size_t fontSize) {

	}
}