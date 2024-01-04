#include "PiPiPager.h"

namespace PiPi {
	PiPiPager::PiPiPager(std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs) {
        this->docs = docs;
	}

    void PiPiPager::Merge(std::vector<size_t>* indexs, std::vector<char>** newPdf) {
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        PdfMemDocument* mergedDocument = new PdfMemDocument();
        PdfPageCollection* mergedPages = &(mergedDocument->GetPages());
        
        for (size_t i = 0; i < indexs->size(); i++) {
            size_t index = indexs->at(i);
            
            if (index >= docs->size()) {
                // 拋錯誤
            }
            
            PiPiOperator* op;
            PdfMemDocument* doc;
            
            std::tie(doc, op) = (*docs)[index];
            
            PdfPageCollection* pages = &(doc->GetPages());
            
            mergedPages->AppendDocumentPages(*doc, 0, pages->GetCount());
        }
        
        *newPdf = new vector<char>();
        PoDoFo::VectorStreamDevice outputStreamDevice(**newPdf);
        
        mergedDocument->Save(outputStreamDevice);
    }

    void PiPiPager::Split(size_t index, std::string instruction, std::vector<std::vector<char>*>** newPdfs) {
        std::vector<std::tuple<PdfMemDocument*, PiPiOperator*>>* docs = this->docs;
        
        if (index >= docs->size()) {
            // 拋錯誤
        }
        
        PiPiOperator* op;
        PdfMemDocument* doc;
        
        std::tie(doc, op) = (*docs)[index];
        
        std::vector<vector<char>*>* dAllNewPdfBytes = new std::vector<vector<char>*>();
        
        std::vector<std::tuple<size_t, size_t>>* instructionPairs = this->ParseSplitInstruction(instruction);
        for (auto iterator = instructionPairs->begin(); iterator != instructionPairs->end(); iterator.operator++()) {
            PdfMemDocument* splitedDocument = new PdfMemDocument();
            PdfPageCollection* splitedPages = &(splitedDocument->GetPages());
            
            size_t s;
            size_t e;
            
            std::tie(s, e) = *iterator;
            
            splitedPages->AppendDocumentPages(*doc, (unsigned int)s, (unsigned int)(e-s));
            
            vector<char>* outputVector = new vector<char>();
            PoDoFo::VectorStreamDevice outputStreamDevice(*outputVector);
            splitedDocument->Save(outputStreamDevice);
            
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
                    // 拋異常
                } catch (std::out_of_range& e) {
                    // 拋異常
                }
                
                if (oneNum < 0) {
                    // 拋異常
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
                    // 拋異常
                } catch (std::out_of_range& e) {
                    // 拋異常
                }
                
                if (oneNum < 0 || twoNum < 0) {
                    // 拋異常
                }
                
                if (oneNum == twoNum) {
                    instructionPairs->push_back(std::make_tuple(oneNum, oneNum + 1));
                } else if (oneNum > twoNum) {
                    instructionPairs->push_back(std::make_tuple(twoNum, oneNum));
                } else {
                    instructionPairs->push_back(std::make_tuple(oneNum, twoNum));
                }
            } else {
                // 拋異常
            }
            
            delete pageInstructions;
        }
        
        delete docInstructions;
        
        return instructionPairs;
    }
}
