#pragma once

#include <podofo/podofo.h>
#include "PiPiAnnotationObserver.h"
#include "PiPiAppearanceUtil.h"
#include "PiPiFieldUtil.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiAppearanceManager {
		public:
			PiPiAppearanceManager(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiAnnotationObserver* annotObserver);
			~PiPiAppearanceManager();

			bool IsOperable();

			void GenerateAppearance();

			void MarkNeedAppearance(std::string fieldName);
			void UnMarkNeedAppearance(std::string fieldName);
			void ClearNeedAppearance();
	
		private:
			PdfMemDocument* document;
			PiPiAnnotationObserver* annotObserver;
			PiPiFontManager* fontManager;

			std::set<std::string>* fieldNames;
	};
}