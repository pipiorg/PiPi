#include "PiPiManagedAnnotations.h"

namespace PiPi {
    PiPiManagedAnnotations::PiPiManagedAnnotations(std::set<PdfAnnotation*>* annots) {
        this->annots = new std::set<PdfAnnotation*>();
        this->annots->insert(annots->begin(), annots->end());
    }

    PiPiManagedAnnotations::PiPiManagedAnnotations(PdfAnnotation* annot) {
        this->annots = new std::set<PdfAnnotation*>();
        this->annots->insert(annot);
    }

    PiPiManagedAnnotations::~PiPiManagedAnnotations() {
        if (this->annots != nullptr) {
            delete this->annots;
            this->annots = nullptr;
        }
    }

    bool PiPiManagedAnnotations::IsEmpty() {
        spdlog::trace("IsEmpty");

        std::set<PdfAnnotation*>* annots = this->annots;
        
        return annots->size() == 0;
    }

    void PiPiManagedAnnotations::ManageAnnotation(PdfAnnotation *annot) {
        spdlog::trace("ManageAnnotation");

        bool exists = false;
        
        for (auto iterator = this->annots->begin(); iterator != this->annots->end(); iterator.operator++()) {
            if (&(annot->GetObject()) == &((*iterator)->GetObject())) {
                exists = true;
                break;
            }
        }
        
        if (exists) {
            return;
        }
        
        this->annots->insert(annot);
    }

    void PiPiManagedAnnotations::UnManageAnnotation(PdfAnnotation *annot) {
        spdlog::trace("UnManageAnnotation");

        std::vector<PdfAnnotation*>* unManageAnnots = new std::vector<PdfAnnotation*>();
        for (auto iterator = this->annots->begin(); iterator != this->annots->end(); iterator.operator++()) {
            if (&(annot->GetObject()) == &((*iterator)->GetObject())) {
                unManageAnnots->push_back(*iterator);
            }
        }
        
        for (auto iterator = unManageAnnots->begin(); iterator != unManageAnnots->end(); iterator.operator++()) {
            this->annots->erase(*iterator);
        }
        
        delete unManageAnnots;
    }

    std::set<PdfAnnotation*>* PiPiManagedAnnotations::AccessAnnotations() {
        spdlog::trace("AccessAnnotations");

        std::set<PdfAnnotation*>* annots = this->annots;
        std::set<PdfAnnotation*>* outAnnots = new std::set<PdfAnnotation*>();
        
        outAnnots->insert(annots->begin(), annots->end());
        
        return outAnnots;
    }

    std::set<PdfObject*>* PiPiManagedAnnotations::AccessObjects() {
        spdlog::trace("AccessObjects");

        std::set<PdfAnnotation*>* annots = this->annots;
        std::set<PdfObject*>* outObjects = new std::set<PdfObject*>();
        
        for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++()) {
            PdfAnnotation* annot = *iterator;
            PdfObject* object = &(annot->GetObject());
            outObjects->insert(object);
        }
        
        return outObjects;
    }
}
