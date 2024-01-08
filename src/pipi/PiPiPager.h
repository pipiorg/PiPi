#pragma once

#include <vector>
#include <tuple>
#include "podofo/podofo.h"
#include "PiPiOperator.h"
#include "PiPiPageException.h"

using namespace PoDoFo;

namespace PiPi {
    class PiPiMultiOperator;

	class PiPiPager {
        friend class PiPiMultiOperator;

		public:
            PiPiPager(std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs);

            bool IsOperable();

            void Merge(std::vector<size_t>* indexs, std::vector<char>** newPdf);
            void Split(size_t index, std::string instruction, std::vector<std::vector<char>*>** newPdfs);

		private:
            bool operable;

            std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs;
        
            std::vector<std::tuple<size_t, size_t>>* ParseSplitInstruction(std::string instruction);
        
            void CopyPage(PdfMemDocument* to, PdfMemDocument* from, unsigned int start, unsigned int end);
            
            void CopyPageDocument(PdfMemDocument* to, PdfMemDocument* from, unsigned int start, unsigned int end);
        
            void CopyPageAcroform(PdfMemDocument* to, PdfMemDocument* from, unsigned int start, unsigned int end);
            void CopyPageAcroformField(PdfMemDocument* to, PdfMemDocument* from, unsigned int start, unsigned int end);
            void CopyPageAcroformFont(PdfMemDocument* to, PdfMemDocument* from, unsigned int start, unsigned int end);
	};
}
