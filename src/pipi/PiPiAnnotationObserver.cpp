#include "PiPiAnnotationObserver.h"

namespace PiPi {
    PiPiAnnotationObserver::PiPiAnnotationObserver() {
        this->annotMap = new std::map<const std::string, const PdfAnnotation*>();
    }
}
