#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>

using namespace PoDoFo;

namespace PiPi
{
	class PiPiOperator;

	class PiPiFontManager
	{
	public:
		PiPiFontManager(PdfMemDocument *document);
		~PiPiFontManager();

		void EmbedFonts();

		const std::string RegisterFont(char *fontBytes, size_t fontSize);

		const PdfFont *AccessFont(const std::string fontName);
		const PdfFont *AccessDefaultFont();

	private:
		PdfMemDocument *document;

		std::set<std::string>* nonStandardFontNames;
		std::map<const std::string, const PdfFont *> *fontMap;

		void Init(PdfMemDocument *document);
		void LoadAcroformFonts();

		void EmbedDocumentFonts();
		void EmbedAcroformFonts();
	};
}
