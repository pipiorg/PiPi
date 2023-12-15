#include "PiPiFieldObserver.h"

namespace PiPi {
    PiPiFieldObserver::PiPiFieldObserver() {
        this->fieldMap = new std::map<const std::string, const PdfField*>();
    }

    PiPiFieldObserver::~PiPiFieldObserver() {
        delete this->fieldMap;
    }
}
