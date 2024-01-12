#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiFontManager.h"
#include "PiPiFieldManager.h"
#include "PiPiAppearanceUtil.h"

using namespace PoDoFo;

namespace PiPi
{
	class PiPiAppearanceManager
	{
	public:
		PiPiAppearanceManager(PdfMemDocument *document, PiPiFontManager *fontManager, PiPiFieldManager *fieldManager);
		~PiPiAppearanceManager();

		bool IsOperable();

		void GenerateAppearance();

		void MarkNeedAppearance(std::string fieldName);
		void UnMarkNeedAppearance(std::string fieldName);
		void ClearNeedAppearance();

	private:
		PdfMemDocument *document;
		PiPiFontManager *fontManager;
		PiPiFieldManager *fieldManager;

		std::set<std::string> *fieldNames;
	};
}
