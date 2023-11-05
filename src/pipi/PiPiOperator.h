#include <iostream>
#include <vector>
#include "podofo/podofo.h"
#include "PiPiFiller.h"
#include "PiPiEditor.h"
#include "PiPiPager.h"

using namespace std;

namespace PiPi {
	class PiPiOperator {
		public:
			PiPiOperator(char* pdfBytes, size_t pdfSize);

			PiPiFiller* getFiller();
			PiPiEditor* getEditor();
			PiPiPager* getPager();


			void finalize(char** newPdfBytes, size_t* newPdfSize);

		private:
			PoDoFo::PdfMemDocument* document;

			PiPiFiller* filler;
			PiPiEditor* editor;
			PiPiPager* pager;

			void init(char* pdfBytes, size_t pdfSize);
	};
}