#pragma once

#include "podofo/podofo.h"
#include "PiPiCommon.h"
#include "PiPiExtractUtil.h"
#include "PiPiFontManager.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiAppearanceUtil {
		public:
			static void GenerateAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot);
		private:
			static void ClearCheckBoxAppearance(PdfAnnotation* annot, PdfCheckBox* field);

			static void GenerateCheckBoxAppearance(PdfAnnotation* annot, PdfCheckBox* field);
			static void GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot, PdfCheckBox* field);
			static void GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot, PdfCheckBox* field);
			static void GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot, PdfCheckBox* field);

			static void GenerateTextBoxAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot, PdfTextBox* field);
	};
}
