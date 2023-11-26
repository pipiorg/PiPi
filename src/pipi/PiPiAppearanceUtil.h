#include "podofo/podofo.h"
#include "PiPiCommon.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiAppearanceUtil {
		public:
			static void GenerateAppearance(PdfAnnotation* annot);
		private:
			static void ClearCheckBoxAppearance(PdfAnnotation* annot);

			static void GenerateCheckBoxAppearance(PdfAnnotation* annot);
			static void GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot);
			static void GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot);
			static void GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot);

			static void GenerateTextBoxAppearance(PdfAnnotation* annot);
	};
}