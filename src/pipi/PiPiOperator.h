#pragma once

#include <iostream>
#include <vector>
#include "podofo/podofo.h"
#include "PiPiFiller.h"
#include "PiPiEditor.h"
#include "PiPiPager.h"
#include "PiPiExtractor.h"
#include "PiPiFontManager.h"

using namespace std;

namespace PiPi {
	class PiPiOperator {
		public:
			PiPiOperator(char* pdfBytes, size_t pdfSize);

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

			void init(char* pdfBytes, size_t pdfSize);
	};
}