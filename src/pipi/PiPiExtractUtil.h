#pragma once

#include <podofo/podofo.h>
#include "PiPiCommon.h"
#include "PiPiColorConverter.h"
#include "PiPiFieldType.h"
#include "PiPiTextHorizontalAlignment.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiExtractUtil {
		public:
			static double ExtractPageWidth(PdfPage* page);
			static double ExtractPageHeight(PdfPage* page);

			static double ExtractPageX(PdfPage* page);
			static double ExtractPageY(PdfPage* page);
        
            static std::string ExtractAnnotationName(PdfAnnotation* annotation);

			static std::string ExtractAnnotationFontName(PdfAnnotation* annotation);
			static float ExtractAnnotationFontSize(PdfAnnotation* annotation);

			static double ExtractAnnotationWidth(PdfAnnotation* annotation);
			static double ExtractAnnotationHeight(PdfAnnotation* annotation);

			static double ExtractAnnotationX(PdfAnnotation* annotation);
			static double ExtractAnnotationY(PdfAnnotation* annotation);
        
            static bool ExtractAnnotationBorderExists(PdfAnnotation* annotation);
            static double ExtractAnnotationBorderWidth(PdfAnnotation* annotation);
            static void ExtractAnnotationBorderColor(PdfAnnotation* annotation, float* red, float* green, float* blue);
        
            static bool ExtractAnnotationBackgroundExists(PdfAnnotation* annotaiton);
            static void ExtractAnnotationBackgroundColor(PdfAnnotation* annotation, float* red, float* green, float* blue);
        
            static void ExtractAnnotationColor(PdfAnnotation* annotation, float* red, float* green, float* blue);

			static PiPiFieldType ExtractAnnotationType(PdfAnnotation* annotation);
        
            static bool ExtractAnnotationTextMultiine(PdfAnnotation* annotation);
            
            static PiPiTextHorizontalAlignment ExtractAnnotationTextHorizontalAlignment(PdfAnnotation* annotation);
	};
}
