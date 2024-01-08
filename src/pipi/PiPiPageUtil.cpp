#include "PiPiPageUtil.h"

namespace PiPi {
    int PiPiPageUtil::SearchPageIndex(PdfMemDocument* document, PdfPage* page) {
        spdlog::trace("SearchPageIndex");

        PdfPageCollection* pages = &(document->GetPages());
        
        unsigned int pageCount = pages->GetCount();
        for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++) {
            if (&(pages->GetPageAt(pageIndex)) == page) {
                return pageIndex;
            }
        }
        
        return -1;
    }

    double PiPiPageUtil::ExtractPageWidth(PdfPage* page) {
        spdlog::trace("ExtractPageWidth");

        Rect rect = page->GetRect();

        double left = rect.GetLeft();
        double right = rect.GetRight();

        double width = right - left;

        return width;
    }

    double PiPiPageUtil::ExtractPageHeight(PdfPage* page) {
        spdlog::trace("ExtractPageHeight");

        Rect rect = page->GetRect();

        double top = rect.GetTop();
        double bottom = rect.GetBottom();

        double height = top - bottom;

        return height;
    }

    double PiPiPageUtil::ExtractPageX(PdfPage* page) {
        spdlog::trace("ExtractPageX");

        Rect rect = page->GetRect();

        double x = rect.GetLeft();

        return x;
    }

    double PiPiPageUtil::ExtractPageY(PdfPage* page) {
        spdlog::trace("ExtractPageY");

        Rect rect = page->GetRect();

        double y = rect.GetTop();

        return y;
    }
}
