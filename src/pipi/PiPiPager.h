#pragma once

#include <vector>
#include <tuple>
#include "podofo/podofo.h"
#include "PiPiOperator.h"

using namespace PoDoFo;

namespace PiPi {
	class PiPiPager {
		public:
            PiPiPager(std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs);

            void Merge(size_t* indexs, size_t count, std::vector<char>** newPdf);
            void Split(size_t index, std::string instruction, std::vector<std::vector<char>*>** newPdfs);

		private:
            std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs;
        
            std::vector<std::tuple<size_t, size_t>>* ParseSplitInstruction(std::string instruction);
	};
}
