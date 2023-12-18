#include "PiPiPageUtil.h"

namespace PiPi {
    int PiPiPageUtil::SearchPageIndex(PdfMemDocument* document, PdfPage* page) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        return SearchPageIndex(pages, page);
    }

    int PiPiPageUtil::SearchPageIndex(PdfPageCollection* pages, PdfPage* page) {
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            if (&(pages->GetPageAt(pageIndex)) == page) {
                return pageIndex;
            }
        }
        return -1;
    }
}
