#include "PiPiPageUtil.h"

namespace PiPi {
    int PiPiPageUtil::SearchPageIndex(PdfMemDocument* document, PdfPage* page) {
        PdfPageCollection& pagesRef = document->GetPages();
        PdfPageCollection* pages = &pagesRef;
        
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            if (&(pages->GetPageAt(pageIndex)) == page) {
                return pageIndex;
            }
        }
        
        return -1;
    }

    double PiPiPageUtil::ExtractPageWidth(PdfPage* page) {
        Rect rect = page->GetRect();

        double left = rect.GetLeft();
        double right = rect.GetRight();

        double width = right - left;

        return width;
    }

    double PiPiPageUtil::ExtractPageHeight(PdfPage* page) {
        Rect rect = page->GetRect();

        double top = rect.GetTop();
        double bottom = rect.GetBottom();

        double height = top - bottom;

        return height;
    }

    double PiPiPageUtil::ExtractPageX(PdfPage* page) {
        Rect rect = page->GetRect();

        double x = rect.GetLeft();

        return x;
    }

    double PiPiPageUtil::ExtractPageY(PdfPage* page) {
        Rect rect = page->GetRect();

        double y = rect.GetTop();

        return y;
    }
}
