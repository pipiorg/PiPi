#pragma once

#include <iostream>
#include <vector>
#include "podofo/podofo.h"
#include "PiPiFiller.h"
#include "PiPiEditor.h"
#include "PiPiPager.h"
#include "PiPiExtractor.h"
#include "PiPiFontManager.h"
#include "PiPiAppearanceManager.h"
#include "PiPiFieldManager.h"

using namespace std;

namespace PiPi {
	class PiPiOperator {
		public:
			PiPiOperator(char* pdfBytes, size_t pdfSize);
            ~PiPiOperator();

			PiPiFiller* GetFiller();
			PiPiEditor* GetEditor();
			PiPiPager* GetPager();
			PiPiExtractor* GetExtractor();
			PiPiFontManager* GetFontManager();

			bool IsOperable();

			void Finalize(char** newPdfBytes, size_t* newPdfSize);

		private:
			PoDoFo::PdfMemDocument* document;

			PiPiFiller* filler;
			PiPiEditor* editor;
			PiPiPager* pager;
			PiPiExtractor* extractor;
			PiPiFontManager* fontManager;
			PiPiAppearanceManager* appearanceManager;
            PiPiFieldManager* fieldManager;
	};
}
