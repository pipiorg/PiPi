#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiStringCommon.h"
#include "PiPiAnnotationUtil.h"
#include "PiPiFontManager.h"
#include "PiPiFieldManager.h"
#include "PiPiAppearanceException.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiAppearanceUtil {
		public:
            static void FlattenAppearance(PiPiFontManager* fontManager, PiPiFieldManager* fieldManager, PdfAnnotation* annot);
			static void GenerateAppearance(PiPiFontManager* fontManager, PiPiFieldManager* fieldManager, PdfAnnotation* annot);
		private:
			static void ClearCheckBoxAppearance(PdfAnnotation* annot);

			static void GenerateCheckBoxAppearance(PdfAnnotation* annot);
			static void GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot);
			static void GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot);
			static void GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot);

			static void GenerateTextBoxAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot);
	};
}
