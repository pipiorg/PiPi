#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>

using namespace PoDoFo;

namespace PiPi
{
	class PiPiFlattenManager
	{
	public:
		PiPiFlattenManager(PdfMemDocument* document);
		~PiPiFlattenManager();

		bool IsFinished();

		void Finish();

		void FlattenAnnot(PdfAnnotation* annot);
		void FlattenImage(PdfAnnotation* annot, PdfImage* image);

	private:
		bool finished;
		PdfMemDocument* document;
		std::map<PdfPage*, PdfPainter*>* painterMap;

		PdfPainter* GetPagePainter(PdfPage* page);
	};
}
