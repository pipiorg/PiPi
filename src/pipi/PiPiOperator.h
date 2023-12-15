#pragma once

#include <iostream>
#include <vector>
#include "podofo/podofo.h"
#include "PiPiFiller.h"
#include "PiPiEditor.h"
#include "PiPiPager.h"
#include "PiPiExtractor.h"
#include "PiPiFontManager.h"
#include "PiPiFieldObserver.h"
#include "PiPiAnnotationObserver.h"

using namespace std;

namespace PiPi {
	class PiPiOperator {
		public:
			PiPiOperator(char* pdfBytes, size_t pdfSize);
            ~PiPiOperator();

			PiPiFiller* getFiller();
			PiPiEditor* getEditor();
			PiPiPager* getPager();
			PiPiExtractor* getExtractor();
			PiPiFontManager* getFontManager();

			bool isOperable();

			void finalize(char** newPdfBytes, size_t* newPdfSize);

		private:
			PoDoFo::PdfMemDocument* document;

			PiPiFiller* filler;
			PiPiEditor* editor;
			PiPiPager* pager;
			PiPiExtractor* extractor;
			PiPiFontManager* fontManager;
            PiPiFieldObserver* fieldObserver;
            PiPiAnnotationObserver* annotObserver;

			void init(char* pdfBytes, size_t pdfSize);
	};
}
