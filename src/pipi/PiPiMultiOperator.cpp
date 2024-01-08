#include "PiPiMultiOperator.h"

namespace PiPi {
    PiPiMultiOperator::PiPiMultiOperator() {
        this->docs = new std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>();
    }

    PiPiMultiOperator::~PiPiMultiOperator() {
        if (this->docs != nullptr) {
            for (auto iterator = this->docs->begin(); iterator != this->docs->end(); iterator.operator++()) {
                PiPiOperator* op;
                PdfMemDocument* doc;
                
                std::tie(doc, op) = *iterator;
                
                if (op != nullptr) {
                    delete op;
                }
            }
            
            delete this->docs;
            this->docs = nullptr;
        }

        if (this->pager != nullptr) {
            this->pager->operable = false;
        }
    }

    size_t PiPiMultiOperator::Add(char *pdfBytes, size_t pdfSize) {
        spdlog::trace("Add");

        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        size_t index = docs->size();
        
        PiPiOperator* op = new PiPiOperator(pdfBytes, pdfSize);
        PdfMemDocument* doc = op->document;
        
        docs->push_back(std::make_tuple(doc, op));
        
        return index;
    }

    PiPiPager* PiPiMultiOperator::GetPager() {
        spdlog::trace("GetPager");

        if (this->pager != nullptr) {
            return this->pager;
        }
        
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        PiPiPager* pager = new PiPiPager(docs);
        
        this->pager = pager;
        
        return this->pager;
    }

    

    PiPiOperator* PiPiMultiOperator::GetOperator(size_t index) {
        spdlog::trace("GetOperator");

        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        if (index >= docs->size()) {
            throw PiPiMultiOperateException(PiPiMultiOperateException::PiPiMultiOperateExceptionCode::IndexOutOfRange);
        }
        
        PdfMemDocument* doc;
        PiPiOperator* op;
        
        std::tie(doc, op) = (*docs)[index];
        
        return op;
    }
}
