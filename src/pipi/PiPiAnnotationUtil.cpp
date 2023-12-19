#include "PiPiAnnotationUtil.h"

namespace PiPi {
    std::map<const std::string, std::set<PdfAnnotation*>*>* PiPiAnnotationUtil::SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SerachAllFieldAnnotation(annotObserver, pages);
    }

    std::map<const std::string, std::set<PdfAnnotation*>*>* PiPiAnnotationUtil::SerachAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages) {
        
        if (annotObserver->isObserved()) {
            std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap;
            bool accessed = annotObserver->accessAll(&annotMap);
            if (accessed) {
                return annotMap;
            }
        }
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* annotMap = new std::map<const std::string, std::set<PdfAnnotation*>*>();

        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;

            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;

            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++) {
                PdfAnnotation& annotRef = annots->GetAnnotAt(annotIndex);
                PdfAnnotation* annot = &annotRef;

                const PdfObject& constObject = annot->GetObject();
                PdfObject& object = const_cast<PdfObject&>(constObject);

                std::unique_ptr<PdfField> fieldPtr;
                bool created = PdfField::TryCreateFromObject(object, fieldPtr);
                if (!created) {
                    continue;
                }

                PdfField* field = fieldPtr.get();

                std::string name = field->GetFullName();

                if ((*annotMap)[name] == nullptr) {
                    (*annotMap)[name] = new std::set<PdfAnnotation*>();
                }

                std::set<PdfAnnotation*>* resAnnots = (*annotMap)[name];
                resAnnots->insert(annot);

                fieldPtr.reset();
            }
        }
        
        annotObserver->observeAll(annotMap);
        delete annotMap;
        
        bool accessed = annotObserver->accessAll(&annotMap);
        if (accessed) {
            return annotMap;
        }
        
        return nullptr;
    }

    std::set<PdfAnnotation*>* PiPiAnnotationUtil::SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfMemDocument* document, std::string fieldName) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchFieldAnnotation(annotObserver, pages, fieldName);
    }

    std::set<PdfAnnotation*>* PiPiAnnotationUtil::SearchFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName) {
        if (annotObserver->isObserved()) {
            std::set<PdfAnnotation*>* resAnnots;
            bool accessed = annotObserver->access(fieldName, &resAnnots);
            if (accessed) {
                return resAnnots;
            }
        }
        
        std::map<const std::string, std::set<PdfAnnotation*>*>* resAnnots = SerachAllFieldAnnotation(annotObserver, pages);
        
        auto findIterator = resAnnots->find(fieldName);
        if (findIterator != resAnnots->end()) {
            return findIterator->second;
        }

        return new std::set<PdfAnnotation*>();
    }

    void PiPiAnnotationUtil::RemoveAllFieldAnnotation(PiPiAnnotationObserver *annotObserver, PdfMemDocument *document) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        RemoveAllFieldAnnotation(annotObserver, pages);
    }

    void PiPiAnnotationUtil::RemoveAllFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages) {
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;
            
            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;
            
            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = annotCount; annotIndex > 0; annotIndex--) {
                annots->RemoveAnnotAt(annotIndex - 1);
            }
        }
        
        annotObserver->observe(PiPiAnnotationObserver::PiPiAnnotationObserveType::Clear, nullptr, nullptr);
    }

    void PiPiAnnotationUtil::RemoveFieldAnnotation(PiPiAnnotationObserver *annotObserver, PdfMemDocument *document, std::string fieldName) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        RemoveFieldAnnotation(annotObserver, pages, fieldName);
    }

    void PiPiAnnotationUtil::RemoveFieldAnnotation(PiPiAnnotationObserver* annotObserver, PdfPageCollection* pages, std::string fieldName) {
        std::set<PdfAnnotation*>* tarAnnots = PiPiAnnotationUtil::SearchFieldAnnotation(annotObserver, pages, fieldName);

        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            PdfPage& pageRef = pages->GetPageAt(pageIndex);
            PdfPage* page = &pageRef;

            PdfAnnotationCollection& annotsRef = page->GetAnnotations();
            PdfAnnotationCollection* annots = &annotsRef;

            std::vector<unsigned int>* removeAnnotIndexs = new std::vector<unsigned int>();

            unsigned int annotCount = annots->GetCount();
            for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++) {
                PdfAnnotation& annotRef = annots->GetAnnotAt(annotIndex);
                PdfAnnotation* annot = &annotRef;

                bool found = std::find(tarAnnots->begin(), tarAnnots->end(), annot) != tarAnnots->end();
                if (found) {
                    removeAnnotIndexs->push_back(annotIndex);
                }
            }

            while (removeAnnotIndexs->size()) {
                unsigned int removeAnnotIndex = removeAnnotIndexs->back();
                
                PdfAnnotation& annotRef = annots->GetAnnotAt(removeAnnotIndex);
                PdfAnnotation* annot = &annotRef;
                
                std::string fieldName = PiPiExtractUtil::ExtractAnnotationName(annot);
                annotObserver->observe(PiPiAnnotationObserver::PiPiAnnotationObserveType::Remove, fieldName, annot);
                
                annots->RemoveAnnotAt(removeAnnotIndex);
                removeAnnotIndexs->pop_back();
            }

            delete removeAnnotIndexs;
        }

        delete tarAnnots;
    }

    void PiPiAnnotationUtil::FlattenAnnotation(PdfAnnotation *annotation) {
        PdfDictionary& dictionary = annotation->GetDictionary();

        std::vector<PdfAppearanceIdentity> apperanceStreams;
        annotation->GetAppearanceStreams(apperanceStreams);

        PdfObject* apperanceStream = nullptr;

        size_t apperanceStreamCount = apperanceStreams.size();
        if (apperanceStreamCount == 1) {
            apperanceStream = const_cast<PdfObject*>(apperanceStreams[0].Object);
        }
        else {
            PdfName as = dictionary.HasKey(PdfName("AS")) ? dictionary.FindKeyAsSafe<PdfName>(PdfName("AS")) : PdfName("Off");
            
            unsigned int normalCount = 0;
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                if (apperanceIdentity.Type == PdfAppearanceType::Normal) {
                    normalCount++;
                }
            }
            
            for (auto iterator = apperanceStreams.begin(); iterator != apperanceStreams.end(); iterator.operator++()) {
                PdfAppearanceIdentity& apperanceIdentity = iterator.operator*();
                
                if (apperanceIdentity.Type == PdfAppearanceType::Normal && apperanceIdentity.State == as) {
                    apperanceStream = const_cast<PdfObject*>(apperanceIdentity.Object);
                }
            }
        }

        if (apperanceStream == nullptr) {
            return;
        }

        PdfObject& apperanceStreanRef = *apperanceStream;

        std::unique_ptr<PdfXObjectForm> xObjectUniquePtr;
        bool xObjectCreated = PdfXObjectForm::TryCreateFromObject(apperanceStreanRef, xObjectUniquePtr);
        if (!xObjectCreated) {
            return;
        }

        PdfXObjectForm* xObject = xObjectUniquePtr.get();
        PdfXObjectForm& xObjectRef = *xObject;

        Rect rect = annotation->GetRect();
        double left = rect.GetLeft();
        double bottom = rect.GetBottom();

        PdfPage* page = annotation->GetPage();
        PdfPage& pageRef = *page;

        PdfPainter* painter = new PdfPainter();
        painter->SetCanvas(pageRef);
        painter->SetClipRect(rect);
        painter->DrawXObject(xObjectRef, left, bottom);
        painter->FinishDrawing();

        delete painter;
    }
}
