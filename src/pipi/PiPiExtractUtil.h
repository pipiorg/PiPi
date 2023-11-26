#pragma once

#include <podofo/podofo.h>
#include "PiPiCommon.h"
#include "PiPiFieldType.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiExtractUtil {
		public:
			static double ExtractPageWidth(PdfPage* page);
			static double ExtractPageHeight(PdfPage* page);

			static double ExtractPageX(PdfPage* page);
			static double ExtractPageY(PdfPage* page);

			static std::string ExtractAnnotationFontName(PdfAnnotation* annotation);
			static float ExtractAnnotationFontSize(PdfAnnotation* annotation);

			static double ExtractAnnotationWidth(PdfAnnotation* annotation);
			static double ExtractAnnotationHeight(PdfAnnotation* annotation);

			static double ExtractAnnotationX(PdfAnnotation* annotation);
			static double ExtractAnnotationY(PdfAnnotation* annotation);

			static PiPiFieldType ExtractAnnotationType(PdfAnnotation* annotation);
	};
}