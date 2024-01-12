#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiFontManageException.h"

using namespace PoDoFo;

namespace PiPi
{
	class PiPiOperator;

	class PiPiFontManager
	{
		friend class PiPiOperator;

	public:
		PiPiFontManager(PdfMemDocument *document);

		bool IsOperable();

		void EmbedFonts();

		const std::string RegisterFont(char *fontBytes, size_t fontSize);

		const PdfFont *AccessFont(const std::string fontName);
		const PdfFont *AccessDefaultFont();

	private:
		bool operable;

		PdfMemDocument *document;

		std::map<const std::string, const PdfFont *> *fontMap;

		void Init(PdfMemDocument *document);
		void LoadAcroformFonts();

		void EmbedDocumentFonts();
		void EmbedAcroformFonts();
	};
}
