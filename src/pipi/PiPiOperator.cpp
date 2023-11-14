#include "PiPiOperator.h"

namespace PiPi {
	PiPiOperator::PiPiOperator(char* pdfBytes, size_t pdfSize) {
		this->init(pdfBytes, pdfSize);
	}

	void PiPiOperator::init(char* pdfBytes, size_t pdfSize) {
		PoDoFo::PdfMemDocument * document = new PoDoFo::PdfMemDocument();
		auto input = make_shared<PoDoFo::SpanStreamDevice>(pdfBytes, pdfSize);
		document->LoadFromDevice(input);
		
		this->pager = new PiPiPager(document);
		this->editor = new PiPiEditor(document);
		this->filler = new PiPiFiller(document);
		this->document = document;
	}

	PiPiFiller* PiPiOperator::getFiller() {
		return this->filler;
	}

	PiPiPager* PiPiOperator::getPager() {
		return this->pager;
	}

	PiPiEditor* PiPiOperator::getEditor() {
		return this->editor;
	}

	bool PiPiOperator::isOperable() {
		return this->document != nullptr;
	}

	void PiPiOperator::finalize(char** newPdfBytes, size_t* newPdfSize) {
		vector<char> outputVector;
		PoDoFo::VectorStreamDevice outputStreamDevice(outputVector);
		this->document->Save(outputStreamDevice);

		*newPdfSize = outputVector.size();
		*newPdfBytes = new char[*newPdfSize];
		for (size_t i = 0; i < *newPdfSize; i++) {
			*(*newPdfBytes + i) = outputVector[i];
		}

		delete this->document;
		this->document = nullptr;
	}
}