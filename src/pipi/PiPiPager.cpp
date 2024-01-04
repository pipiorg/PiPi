#include "PiPiPager.h"

namespace PiPi {
	PiPiPager::PiPiPager(std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs) {
        this->docs = docs;
	}

    void PiPiPager::Merge(std::vector<size_t>* indexs, std::vector<char>** newPdf) {
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        PdfMemDocument* mergedDoc = new PdfMemDocument();
        
        for (size_t i = 0; i < indexs->size(); i++) {
            size_t index = indexs->at(i);
            
            if (index >= docs->size()) {
                throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::IndexOutOfRange);
            }
            
            PiPiOperator* op;
            PdfMemDocument* doc;
            
            std::tie(doc, op) = (*docs)[index];
            
            PdfPageCollection* pages = &(doc->GetPages());
            this->CopyPage(mergedDoc, doc, 0, pages->GetCount());
        }
        
        *newPdf = new vector<char>();
        PoDoFo::VectorStreamDevice outputStreamDevice(**newPdf);
        
        mergedDoc->Save(outputStreamDevice);
    }

    void PiPiPager::Split(size_t index, std::string instruction, std::vector<std::vector<char>*>** newPdfs) {
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        if (index >= docs->size()) {
            throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::IndexOutOfRange);
        }
        
        PiPiOperator* op;
        PdfMemDocument* doc;
        
        std::tie(doc, op) = (*docs)[index];
        
        std::vector<vector<char>*>* dAllNewPdfBytes = new std::vector<vector<char>*>();
        
        std::vector<std::tuple<size_t, size_t>>* instructionPairs = this->ParseSplitInstruction(instruction);
        for (auto iterator = instructionPairs->begin(); iterator != instructionPairs->end(); iterator.operator++()) {
            PdfMemDocument* splitedDoc = new PdfMemDocument();
            PdfPageCollection* splitedPages = &(splitedDoc->GetPages());
            
            size_t s;
            size_t e;
            
            std::tie(s, e) = *iterator;
            
            this->CopyPage(doc, splitedDoc, (unsigned int)s, (unsigned int)e);
            
            vector<char>* outputVector = new vector<char>();
            PoDoFo::VectorStreamDevice outputStreamDevice(*outputVector);
            splitedDoc->Save(outputStreamDevice);
            
            dAllNewPdfBytes->push_back(outputVector);
        }
        
        delete instructionPairs;
        
        *newPdfs = new std::vector<std::vector<char>*>();
        
        for (size_t i = 0; i < dAllNewPdfBytes->size(); i++) {
            vector<char>* dNewPdfBytes = dAllNewPdfBytes->at(i);
            (*newPdfs)->push_back(dNewPdfBytes);
        }
        
        delete dAllNewPdfBytes;
    }

    std::vector<std::tuple<size_t, size_t>>* PiPiPager::ParseSplitInstruction(std::string instruction) {
        std::vector<std::tuple<size_t, size_t>>* instructionPairs = new std::vector<std::tuple<size_t, size_t>>();
        
        std::vector<std::string>* docInstructions =  PiPiStringCommon::split(instruction, ",");
        
        for (auto iterator = docInstructions->begin(); iterator != docInstructions->end(); iterator.operator++()) {
            std::string docInstruction = *iterator;
            
            std::vector<std::string>* pageInstructions = PiPiStringCommon::split(docInstruction, "-");
            
            if (pageInstructions->size() == 1) {
                std::string one = (*pageInstructions)[0];
                
                int oneNum = -1;
                
                try {
                    oneNum = std::stoi(one);
                } catch (std::invalid_argument& e) {
                    throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
                } catch (std::out_of_range& e) {
                    throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
                }
                
                if (oneNum < 0) {
                    throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
                }
                
                instructionPairs->push_back(std::make_tuple(oneNum, oneNum + 1));
            } else if (pageInstructions->size() == 2) {
                std::string one = (*pageInstructions)[0];
                std::string two = (*pageInstructions)[1];
                
                int oneNum = -1;
                int twoNum = -2;
                
                try {
                    oneNum = std::stoi(one);
                    twoNum = std::stoi(two);
                } catch (std::invalid_argument& e) {
                    throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
                } catch (std::out_of_range& e) {
                    throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
                }
                
                if (oneNum < 0 || twoNum < 0) {
                    throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
                }
                
                if (oneNum == twoNum) {
                    instructionPairs->push_back(std::make_tuple(oneNum, oneNum + 1));
                } else if (oneNum > twoNum) {
                    instructionPairs->push_back(std::make_tuple(twoNum, oneNum));
                } else {
                    instructionPairs->push_back(std::make_tuple(oneNum, twoNum));
                }
            } else {
                throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
            }
            
            delete pageInstructions;
        }
        
        delete docInstructions;
        
        return instructionPairs;
    }

    void PiPiPager::CopyPage(PdfMemDocument *to, PdfMemDocument* from, unsigned int start, unsigned int end) {
        this->CopyPageDocument(to, from, start, end);
        this->CopyPageAcroform(to, from, start, end);
    }

    void PiPiPager::CopyPageDocument(PdfMemDocument *to, PdfMemDocument* from, unsigned int start, unsigned int end) {
        PdfPageCollection* toPages = &(to->GetPages());
        toPages->AppendDocumentPages(*from, start, end - start);
    }

    void PiPiPager::CopyPageAcroform(PdfMemDocument *to, PdfMemDocument* from, unsigned int start, unsigned int end) {
        
    }
}
