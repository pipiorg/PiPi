#include "PiPiManagedFields.h"

namespace PiPi {
    PiPiManagedFields::PiPiManagedFields(std::set<PdfField*>* fields) {
        this->real = false;
        
        this->fields = new std::set<PdfField*>();
        this->fields->insert(fields->begin(), fields->end());
    }

    PiPiManagedFields::PiPiManagedFields(PdfField* field, bool real) {
        this->real = real;
        
        this->fields = new std::set<PdfField*>();
        this->fields->insert(field);
    }

    PiPiManagedFields::~PiPiManagedFields() {
        if (this->fields != nullptr) {
            delete this->fields;
            this->fields = nullptr;
        }
    }

    bool PiPiManagedFields::IsReal() {
        return this->real;
    }

    bool PiPiManagedFields::IsEmpty() {
        std::set<PdfField*>* fields = this->fields;
        return fields->size() == 0;
    }

    void PiPiManagedFields::ManageField(PdfField *field) {
        bool exists = false;
        
        for (auto iterator = this->fields->begin(); iterator != this->fields->end(); iterator.operator++()) {
            if (&(field->GetObject()) == &((*iterator)->GetObject())) {
                exists = true;
                break;
            }
        }
        
        if (exists) {
            return;
        }
        
        this->fields->insert(field);
    }

    void PiPiManagedFields::UnManageField(PdfField *field) {
        std::vector<PdfField*>* unManageFields = new std::vector<PdfField*>();
        for (auto iterator = this->fields->begin(); iterator != this->fields->end(); iterator.operator++()) {
            if (&(field->GetObject()) == &((*iterator)->GetObject())) {
                unManageFields->push_back(*iterator);
            }
        }
        
        for (auto iterator = unManageFields->begin(); iterator != unManageFields->end(); iterator.operator++()) {
            this->fields->erase(*iterator);
        }
        
        delete unManageFields;
    }

    std::set<PdfField*>* PiPiManagedFields::AccessFields() {
        std::set<PdfField*>* fields = this->fields;
        std::set<PdfField*>* outFields = new std::set<PdfField*>();
        
        outFields->insert(fields->begin(), fields->end());
        
        return outFields;
    }

    std::set<PdfObject*>* PiPiManagedFields::AccessObjects() {
        std::set<PdfField*>* fields = this->fields;
        std::set<PdfObject*>* outObjects = new std::set<PdfObject*>();
        
        for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++()) {
            PdfField* field = *iterator;
            PdfObject* object = &(field->GetObject());
            outObjects->insert(object);
        }
        
        return outObjects;
    }
}
