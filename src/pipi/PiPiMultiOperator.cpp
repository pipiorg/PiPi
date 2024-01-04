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
    }

    size_t PiPiMultiOperator::Add(char *pdfBytes, size_t pdfSize) {
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        size_t index = docs->size();
        
        PiPiOperator* op = new PiPiOperator(pdfBytes, pdfSize);
        PdfMemDocument* doc = op->document;
        
        docs->push_back(std::make_tuple(doc, op));
        
        return index;
    }

    PiPiPager* PiPiMultiOperator::GetPager() {
        if (this->pager != nullptr) {
            return this->pager;
        }
        
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        PiPiPager* pager = new PiPiPager(docs);
        
        this->pager = pager;
        
        return this->pager;
    }

    

    PiPiOperator* PiPiMultiOperator::GetOperator(size_t index) {
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        if (index >= docs->size()) {
            // 拋錯誤
        }
        
        PdfMemDocument* doc;
        PiPiOperator* op;
        
        std::tie(doc, op) = (*docs)[index];
        
        return op;
    }
}
