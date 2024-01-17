#include "PiPiFlattenManager.h"

namespace PiPi {
	PiPiFlattenManager::PiPiFlattenManager(PdfMemDocument* document) {
		this->document = document;
		this->painterMap = new std::map<PdfPage*, PdfPainter*>();
		this->finished = false;
	}

	PiPiFlattenManager::~PiPiFlattenManager() {
		if (this->painterMap != nullptr) {
			for (auto iterator = painterMap->begin(); iterator != painterMap->end(); iterator.operator++()) {
				PdfPainter* painter = iterator->second;
				painter->FinishDrawing();
				delete painter;
			}

			delete this->painterMap;
		}
	}

	void PiPiFlattenManager::Finish() {
		spdlog::trace("Finish");

		std::map<PdfPage*, PdfPainter*>* painterMap = this->painterMap;

		for (auto iterator = painterMap->begin(); iterator != painterMap->end(); iterator.operator++()) {
			PdfPainter* painter = iterator->second;
			painter->FinishDrawing();
			delete painter;
		}

		painterMap->clear();
	}

	void PiPiFlattenManager::FlattenAnnot(PdfAnnotation* annot) {
		spdlog::trace("FlattenAnnot");

		PdfPage* page = annot->GetPage();
		PdfPainter* painter = this->GetPagePainter(page);

		PdfDictionary& dictionary = annot->GetDictionary();

		std::vector<PdfAppearanceIdentity> apperanceStreams;
		annot->GetAppearanceStreams(apperanceStreams);

		PdfObject* apperanceStream = nullptr;

		size_t apperanceStreamCount = apperanceStreams.size();
		if (apperanceStreamCount == 1)
		{
			apperanceStream = const_cast<PdfObject*>(apperanceStreams[0].Object);
		}
		else
		{
			PdfName as = dictionary.HasKey(PdfName("AS")) ? dictionary.FindKeyAsSafe<PdfName>(PdfName("AS")) : PdfName("Off");

			unsigned int normalCount = 0;
			for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++())
			{
				PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
				if (apperanceIdentity.Type == PdfAppearanceType::Normal)
				{
					normalCount++;
				}
			}

			for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++())
			{
				PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();

				if (apperanceIdentity.Type == PdfAppearanceType::Normal && apperanceIdentity.State == as)
				{
					apperanceStream = const_cast<PdfObject*>(apperanceIdentity.Object);
				}
			}
		}

		if (apperanceStream == nullptr)
		{
			return;
		}

		PdfObject& apperanceStreanRef = *apperanceStream;

		std::unique_ptr<PdfXObjectForm> xObjectUniquePtr;
		bool xObjectCreated = PdfXObjectForm::TryCreateFromObject(apperanceStreanRef, xObjectUniquePtr);
		if (!xObjectCreated)
		{
			throw PiPiManageFlattenException(PiPiManageFlattenException::PiPiManageFlattenExceptionCode::CreateFormObjectFromAppearanceFail);
		}

		PdfXObjectForm* xObject = xObjectUniquePtr.get();

		Rect rect = annot->GetRect();
		double left = rect.GetLeft();
		double bottom = rect.GetBottom();

		painter->SetCanvas(*page);
		painter->DrawXObject(*xObject, left, bottom);
	}

	void PiPiFlattenManager::FlattenImage(PdfAnnotation* annot, PdfImage* image) {
		spdlog::trace("FlattenImage");

		PdfPage* page = annot->GetPage();
		PdfPainter* painter = this->GetPagePainter(page);

		Rect annotRect = annot->GetRect();

		double annotBottom = annotRect.GetBottom();
		double annotTop = annotRect.GetTop();
		double annotLeft = annotRect.GetLeft();
		double annotRight = annotRect.GetRight();

		double annotWidth = annotRight - annotLeft;
		double annotHeight = annotTop - annotBottom;

		Rect imageRect = image->GetRect();

		double imageBottom = imageRect.GetBottom();
		double imageTop = imageRect.GetTop();
		double imageLeft = imageRect.GetLeft();
		double imageRight = imageRect.GetRight();

		double imageWidth = imageRight - imageLeft;
		double imageHeight = imageTop - imageBottom;

		double scale = std::min(annotWidth / imageWidth, annotHeight / imageHeight);

		double scaledImageWidth = imageWidth * scale;
		double scaledImageHeight = imageHeight * scale;

		double left = annotLeft + annotWidth / 2 - scaledImageWidth / 2;
		double bottom = annotBottom + annotHeight / 2 - scaledImageHeight / 2;

		painter->SetCanvas(*page);
		painter->DrawImage(*image, left, bottom, scale, scale);
	}

	bool PiPiFlattenManager::IsFinished() {
		spdlog::trace("IsFinished");

		return this->finished;
	}

	PdfPainter* PiPiFlattenManager::GetPagePainter(PdfPage* page) {
		spdlog::trace("GetPagePainter");

		std::map<PdfPage*, PdfPainter*>* painterMap = this->painterMap;

		auto findIterator = painterMap->find(page);
		if (findIterator != painterMap->end()) {
			return findIterator->second;
		}

		page->GetContents()->GetStreamForAppending(PdfStreamAppendFlags::NoSaveRestorePrior | PdfStreamAppendFlags::Prepend).GetOutputStream().Write("q\n");
		page->GetContents()->GetStreamForAppending(PdfStreamAppendFlags::NoSaveRestorePrior).GetOutputStream().Write("Q\n");

		PdfPainter* painter = new PdfPainter(PdfPainterFlags::NoSaveRestorePrior);
		painterMap->insert(std::pair<PdfPage*, PdfPainter*>(page, painter));

		return painter;
	}
}
