#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>

using namespace PoDoFo;

namespace PiPi
{
  class PiPiManagedAnnotations
  {
  public:
    PiPiManagedAnnotations(std::set<PdfAnnotation *> *annots);
    PiPiManagedAnnotations(PdfAnnotation *annot);

    ~PiPiManagedAnnotations();

    bool IsEmpty();

    void ManageAnnotation(PdfAnnotation *annot);
    void UnManageAnnotation(PdfAnnotation *annot);

    std::set<PdfAnnotation *> *AccessAnnotations();
    std::set<PdfObject *> *AccessObjects();

  private:
    std::set<PdfAnnotation *> *annots;
  };
}
