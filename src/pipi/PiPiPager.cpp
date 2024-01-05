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
        this->CopyPageAcroformField(to, from, start, end);
        this->CopyPageAcroformFont(to, from, start, end);
    }

    void PiPiPager::CopyPageAcroformField(PdfMemDocument *to, PdfMemDocument *from, unsigned int start, unsigned int end) {
        PdfAcroForm* acroform = &(to->GetOrCreateAcroForm());
        PdfDictionary* acroformDict = &(acroform->GetDictionary());
        
        PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        if (acroformFieldsObj == nullptr) {
            acroformDict->AddKey(PdfName("Fields"), PdfArray());
            acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        }
        
        PdfArray* acroformFields = &(acroformFieldsObj->GetArray());
        
        PdfPageCollection* toPages = &(to->GetPages());
        for (unsigned int i = start; i < end; i++) {
            PdfPage* toPage = &(toPages->GetPageAt(i));
            PdfAnnotationCollection* toAnnots = &(toPage->GetAnnotations());
            for (unsigned int j = 0; j < toAnnots->GetCount(); j++) {
                PdfAnnotation* toAnnot = &(toAnnots->GetAnnotAt(j));
                PdfObject* toObj = &(toAnnot->GetObject());
                PdfDictionary* toDict = &(toObj->GetDictionary());
                
                std::unique_ptr<PdfField> toFieldPtr;
                bool created = PdfField::TryCreateFromObject(*toObj, toFieldPtr);
                if (!created) {
                    continue;
                }
                
                PdfObject* toRootObj = toObj;
                PdfObject* toParentObj = toDict->FindKey(PdfName("Parent"));
                while (toParentObj != nullptr) {
                    toRootObj = toParentObj;
                    toParentObj = (&(toRootObj->GetDictionary()))->FindKey(PdfName("Parent"));
                }
                
                bool added = false;
                for (unsigned int k = 0; k < acroformFields->size(); k++) {
                    PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(k));
                    if (acroformFieldObj == toRootObj) {
                        added = true;
                        break;
                    }
                }
                
                if (added) {
                    continue;
                }
                
                acroformFields->AddIndirect(*toRootObj);
            }
        }
    }

    void PiPiPager::CopyPageAcroformFont(PdfMemDocument *to, PdfMemDocument *from, unsigned int start, unsigned int end) {
        std::map<std::string, std::string>* fontMap = new std::map<std::string, std::string>();
        
        PdfAcroForm* fromAcroform = &(from->GetOrCreateAcroForm());
        PdfDictionary* fromAcroformDict = &(fromAcroform->GetDictionary());
        
        PdfObject* fromAcroformDrObj = fromAcroformDict->FindKey(PdfName("DR"));
        PdfDictionary* fromAcroformDr = &(fromAcroformDrObj->GetDictionary());
        
        PdfObject* fromAcroformDrFontObj = fromAcroformDr->FindKey(PdfName("Font"));
        PdfDictionary* fromAcroformDrFont = &(fromAcroformDrFontObj->GetDictionary());
        
        PdfPageCollection* fromPages = &(from->GetPages());
        for (unsigned int i = 0; i < fromPages->GetCount(); i++) {
            PdfPage* fromPage = &(fromPages->GetPageAt(i));
            PdfAnnotationCollection* fromAnnots = &(fromPage->GetAnnotations());
            for (unsigned int j = 0; j < fromAnnots->GetCount(); j++) {
                PdfAnnotation* toAnnot = &(fromAnnots->GetAnnotAt(j));
                
                std::string fromFontName = PiPiAnnotationUtil::ExtractAnnotationFontName(toAnnot);
                if (fromFontName == "") {
                    continue;
                }
                
                auto fontMapFindIterator = fontMap->find(fromFontName);
                if (fontMapFindIterator != fontMap->end()) {
                    continue;
                }
                
                PdfObject* fromFontObj = fromAcroformDrFont->FindKey(PdfName(fromFontName));
                if (fromFontObj == nullptr) {
                    continue;
                }
                
                PdfDictionary* fromFontDict = &(fromFontObj->GetDictionary());
                
                PdfObject* fromFontBaseFontObj = fromFontDict->FindKey(PdfName("BaseFont"));
                if (fromFontBaseFontObj == nullptr) {
                    continue;
                }
                
                const PdfName* fromFontBaseFont = &(fromFontBaseFontObj->GetName());
                
                std::string fromBaseFontName = fromFontBaseFont->GetString();
                
                fontMap->insert(std::pair<std::string, std::string>(fromFontName, fromBaseFontName));
            }
        }
        
        PdfIndirectObjectList* toIndirectObjectList = &(to->GetObjects());
        
        PdfAcroForm* toAcroform = &(to->GetOrCreateAcroForm());
        PdfDictionary* toAcroformDict = &(toAcroform->GetDictionary());
        
        PdfObject* toAcroformDrObj = toAcroformDict->FindKey(PdfName("DR"));
        PdfDictionary* toAcroformDr = &(toAcroformDrObj->GetDictionary());
        
        PdfObject* toAcroformDrFontObj = toAcroformDr->FindKey(PdfName("Font"));
        PdfDictionary* toAcroformDrFont = &(toAcroformDrFontObj->GetDictionary());
        
        for (auto iterator = fontMap->begin(); iterator != fontMap->end(); iterator.operator++()) {
            std::string fontName = iterator->first;
            std::string baseFontName = iterator->second;
            
            PdfObject* fontObj = nullptr;
            for (auto toIndirectIterator = toIndirectObjectList->begin(); toIndirectIterator != toIndirectObjectList->end(); toIndirectIterator.operator++()) {
                PdfObject* toIndirectObject = *toIndirectIterator;
                
                if (!toIndirectObject->IsDictionary()) {
                    continue;
                }
                
                PdfDictionary* toIndirectDict = &(toIndirectObject->GetDictionary());
                
                if (!toIndirectDict->HasKey(PdfName("Type"))) {
                    continue;
                }
                
                PdfObject* toIndirectTypeObj = toIndirectDict->FindKey(PdfName("Type"));
                
                if (!toIndirectTypeObj->IsName()) {
                    continue;
                }
                
                const PdfName* toIndirectType = &(toIndirectTypeObj->GetName());
                
                if (*toIndirectType != PdfName("Font")) {
                    continue;
                }
                
                PdfObject* toIndirectBaseFontEncoding = toIndirectDict->FindKey(PdfName("Encoding"));
                if (toIndirectBaseFontEncoding == nullptr) {
                    continue;
                }
                
                PdfObject* toIndirectBaseFontObj = toIndirectDict->FindKey(PdfName("BaseFont"));
                if (toIndirectBaseFontObj == nullptr) {
                    continue;
                }
                
                const PdfName* toIndirectBaseFont = &(toIndirectBaseFontObj->GetName());
                
                const std::string toBaseFontName = toIndirectBaseFont->GetString();
                
                if (toBaseFontName == baseFontName) {
                    fontObj = toIndirectObject;
                }
            }
            
            if (fontObj == nullptr) {
                continue;
            }
            
            toAcroformDrFont->AddKeyIndirect(PdfName(fontName), *fontObj);
        }
        
        delete fontMap;
    }
}
