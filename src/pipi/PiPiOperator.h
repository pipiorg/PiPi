#pragma once

#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>
#include <podofo/podofo.h>
#include "PiPiLogCommon.h"
#include "PiPiFiller.h"
#include "PiPiEditor.h"
#include "PiPiExtractor.h"
#include "PiPiFontManager.h"
#include "PiPiAppearanceManager.h"
#include "PiPiFlattenManager.h"
#include "PiPiFieldManager.h"
#include "PiPiFieldStyleManager.h"

using namespace std;

namespace PiPi
{
	class PiPiMultiOperator;

	class PiPiOperator
	{
		friend class PiPiMultiOperator;

	public:
		PiPiOperator(char* pdfBytes, size_t pdfSize);
		~PiPiOperator();

		PiPiFiller* GetFiller();
		PiPiEditor* GetEditor();
		PiPiExtractor* GetExtractor();
		PiPiFontManager* GetFontManager();

		bool IsOperable();

		void Finalize(std::vector<char>** newPdf);

	private:
		PoDoFo::PdfMemDocument* document;

		PiPiFiller* filler;
		PiPiEditor* editor;
		PiPiExtractor* extractor;
		PiPiFontManager* fontManager;
		PiPiAppearanceManager* appearanceManager;
		PiPiFlattenManager* flattenManager;
		PiPiFieldManager* fieldManager;
		PiPiFieldStyleManager* fieldStyleManager;
	};
}
