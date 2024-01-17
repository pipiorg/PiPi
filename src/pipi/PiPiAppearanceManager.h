#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiFontManager.h"
#include "PiPiFieldManager.h"
#include "PiPiAppearanceException.h"

using namespace PoDoFo;

namespace PiPi
{
	class PiPiAppearanceManager
	{
	public:
		PiPiAppearanceManager(PdfMemDocument* document, PiPiFontManager* fontManager, PiPiFieldManager* fieldManager);
		~PiPiAppearanceManager();

		void GenerateAppearance();
		void GenerateAppearance(std::string fieldName);

		void MarkNeedAppearance(std::string fieldName);
		void UnMarkNeedAppearance(std::string fieldName);
		void ClearNeedAppearance();

	private:
		PdfMemDocument* document;
		PiPiFontManager* fontManager;
		PiPiFieldManager* fieldManager;

		std::set<std::string>* fieldNames;

		void GenerateAppearance(PiPiFontManager* fontManager, PiPiFieldManager* fieldManager, PdfAnnotation* annot);
		void ClearCheckBoxAppearance(PdfAnnotation* annot);

		void GenerateCheckBoxAppearance(PdfAnnotation* annot);
		void GenerateCheckBoxNormalCheckAppearance(PdfAnnotation* annot);
		void GenerateCheckBoxDownCheckAppearance(PdfAnnotation* annot);
		void GenerateCheckBoxDownUnCheckAppearance(PdfAnnotation* annot);

		void GenerateTextBoxAppearance(PiPiFontManager* fontManager, PdfAnnotation* annot);
	};
}
