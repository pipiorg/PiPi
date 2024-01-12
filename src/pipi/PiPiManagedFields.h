#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>

using namespace PoDoFo;

namespace PiPi
{
  class PiPiManagedFields
  {
  public:
    PiPiManagedFields(PdfField *field, bool real);
    PiPiManagedFields(std::set<PdfField *> *fields);

    ~PiPiManagedFields();

    bool IsReal();
    bool IsEmpty();

    void ManageField(PdfField *field);
    void UnManageField(PdfField *field);

    std::set<PdfField *> *AccessFields();
    std::set<PdfObject *> *AccessObjects();

  private:
    bool real;

    std::set<PdfField *> *fields;
  };
}
