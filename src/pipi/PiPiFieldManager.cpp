#include "PiPiFieldManager.h"

namespace PiPi
{
  PiPiFieldManager::PiPiFieldManager(PdfMemDocument *document)
  {
    this->InitDocument(document);
    this->InitFieldMap();
    this->InitAnnotationMap();
    this->InitFieldBridgeMap();
    this->InitAnnotationBridgeMap();
  }

  PiPiFieldManager::~PiPiFieldManager()
  {
    if (this->fieldMap != nullptr)
    {
      for (auto mapIterator = this->fieldMap->begin(); mapIterator != this->fieldMap->end(); mapIterator.operator++())
      {
        delete mapIterator->second;
        mapIterator->second = nullptr;
      }

      delete this->fieldMap;
      this->fieldMap = nullptr;
    }

    if (this->annotMap != nullptr)
    {
      for (auto mapIterator = this->annotMap->begin(); mapIterator != this->annotMap->end(); mapIterator.operator++())
      {
        delete mapIterator->second;
        mapIterator->second = nullptr;
      }

      delete this->annotMap;
      this->annotMap = nullptr;
    }

    if (this->fieldBridgeMap != nullptr)
    {
      delete this->fieldBridgeMap;
      this->fieldBridgeMap = nullptr;
    }

    if (this->annotBridgeMap != nullptr)
    {
      delete this->annotBridgeMap;
      this->annotBridgeMap = nullptr;
    }
  }

  bool PiPiFieldManager::IsOperable()
  {
    spdlog::trace("IsOperable");

    return this->document != nullptr;
  }

  std::map<const std::string, std::set<PdfField *> *> *PiPiFieldManager::SearchAllField()
  {
    spdlog::trace("SearchAllField");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;
    std::map<const std::string, std::set<PdfField *> *> *outFieldMap = new std::map<const std::string, std::set<PdfField *> *>();
    for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++())
    {
      const std::string fieldName = mapIterator->first;
      PiPiManagedFields *managedFields = mapIterator->second;

      if (!managedFields->IsReal())
      {
        continue;
      }

      std::set<PdfField *> *fields = managedFields->AccessFields();
      std::set<PdfField *> *outFields = new std::set<PdfField *>();

      outFields->insert(fields->begin(), fields->end());
      delete fields;

      outFieldMap->insert(std::pair<const std::string, std::set<PdfField *> *>(fieldName, outFields));
    }

    return outFieldMap;
  }

  std::set<PdfField *> *PiPiFieldManager::SearchField(std::string fieldName)
  {
    spdlog::trace("SearchField");

    auto findIterator = this->fieldMap->find(fieldName);
    if (findIterator == this->fieldMap->end())
    {
      return new std::set<PdfField *>();
    }

    PiPiManagedFields *managedFields = findIterator->second;

    std::set<PdfField *> *fields = managedFields->AccessFields();
    std::set<PdfField *> *outFields = new std::set<PdfField *>();

    outFields->insert(fields->begin(), fields->end());

    delete fields;

    return outFields;
  }

  void PiPiFieldManager::RemoveAllField()
  {
    spdlog::trace("RemoveAllField");

    PdfMemDocument *document = this->document;

    PdfAcroForm *acroform = &(document->GetOrCreateAcroForm());
    PdfDictionary *acroformDict = &(acroform->GetDictionary());

    PdfObject *acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
    PdfArray *acroformFields = &(acroformFieldsObj->GetArray());

    long acroformFieldsCount = acroformFields->size();
    for (long i = acroformFieldsCount - 1; i >= 0; i--)
    {
      acroformFields->RemoveAt((unsigned int)i);
    }

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;
    for (auto iterator = fieldMap->begin(); iterator != fieldMap->end(); iterator.operator++())
    {
      delete iterator->second;
      iterator->second = nullptr;
    }

    fieldMap->clear();

    std::map<PdfObject *, PdfField *> *fieldBridgeMap = this->fieldBridgeMap;
    fieldBridgeMap->clear();

    this->RemoveAllFieldAnnotation();
  }

  void PiPiFieldManager::RemoveField(std::string fieldName)
  {
    spdlog::trace("RemoveField");

    this->RemoveField(fieldName, -1);
  }

  void PiPiFieldManager::RemoveField(std::string fieldName, long pageIndex)
  {
    spdlog::trace("RemoveField");

    this->RemoveField(fieldName, pageIndex, -1, -1);
  }

  void PiPiFieldManager::RemoveField(std::string fieldName, long pageIndex, double x, double y)
  {
    spdlog::trace("RemoveField");

    this->RemoveField(fieldName, pageIndex, x, y, -1, -1);
  }

  void PiPiFieldManager::RemoveField(std::string fieldName, long pageIndex, double x, double y, double width, double height)
  {
    spdlog::trace("RemoveField");

    PdfMemDocument *document = this->document;
    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;

    auto findIterator = fieldMap->find(fieldName);
    if (findIterator == fieldMap->end())
    {
      return;
    }

    PiPiManagedFields *managedFields = findIterator->second;
    if (!managedFields->IsReal())
    {
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::UnsupportRemoveFakeField);
    }

    std::set<PdfField *> *fields = managedFields->AccessFields();
    for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
    {
      PdfField *field = *iterator;
      PdfAnnotation *annot = this->BridgeFieldToAnnotation(field);

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (aPageIndex == pageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        PdfObject *fieldObj = &(field->GetObject());
        this->InnerRemoveField(fieldObj);
      }
    }

    delete fields;

    this->RemoveFieldAnnotation(fieldName, pageIndex, x, y, width, height);
  }

  PdfField *PiPiFieldManager::CreateField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height)
  {
    spdlog::trace("CreateField");

    if (this->IsDuplicateFieldExists(fieldName))
    {
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::DuplicateFieldExists);
    }

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;
    auto findIterator = fieldMap->find(fieldName);
    if (findIterator != fieldMap->end())
    {
      PiPiManagedFields *managedFields = findIterator->second;

      if (managedFields->IsReal())
      {
        std::set<PdfField *> *fields = managedFields->AccessFields();

        size_t fieldCount = fields->size();
        if (fieldCount == 1)
        {
          PdfField *field = *(fields->begin());
          PdfObject *fieldObj = &(field->GetObject());
          this->ExpandField(fieldObj);
        }

        delete fields;
      }
    }

    PdfObject *fieldObj = this->InnerCreateRealField(fieldName, type, pageIndex, x, y, width, height);
    PdfField *field = this->BridgeObjectToField(fieldObj);

    return field;
  }

  void PiPiFieldManager::RenameField(std::string oldFieldName, std::string newFieldName)
  {
    spdlog::trace("RenameField");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;

    auto oldFindIterator = fieldMap->find(oldFieldName);
    if (oldFindIterator == fieldMap->end())
    {
      return;
    }

    PiPiManagedFields *oldManagedFields = oldFindIterator->second;

    if (!oldManagedFields->IsReal())
    {
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::UnsupportRenameFakeField);
    }

    std::set<PdfField *> *oldFields = oldManagedFields->AccessFields();

    if (this->IsDuplicateFieldExists(newFieldName))
    {
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::DuplicateFieldExists);
    }

    std::vector<std::string> *splits = PiPiStringCommon::split(newFieldName, ".");

    std::string lastFieldName = splits->back();
    splits->pop_back();

    std::string partialFieldName = PiPiStringCommon::join(splits, ".");
    delete splits;

    std::map<PdfObject *, PdfField *> *renameFieldMap = new std::map<PdfObject *, PdfField *>();

    auto newFindIterator = fieldMap->find(newFieldName);
    if (newFindIterator == fieldMap->end())
    {
      PdfObject *aggregateFieldObj = nullptr;
      if (oldFields->size() == 1)
      {
        PdfField *oldField = *(oldFields->begin());
        PdfObject *oldFieldObj = &(oldField->GetObject());
        PdfDictionary *oldFieldDict = &(oldField->GetDictionary());

        this->InnerRemoveField(oldFieldObj);

        renameFieldMap->insert(std::pair<PdfObject *, PdfField *>(oldFieldObj, oldField));

        oldFieldDict->RemoveKey(PdfName("T"));
        oldFieldDict->AddKey(PdfName("T"), PdfString(lastFieldName));

        aggregateFieldObj = &(oldField->GetObject());
      }
      else
      {
        PdfField *oldField = *(oldFields->begin());
        PdfDictionary *oldFieldDict = &(oldField->GetDictionary());

        PdfObject *oldParentFieldObj = oldFieldDict->FindKey(PdfName("Parent"));
        PdfDictionary *oldParentFieldDict = &(oldParentFieldObj->GetDictionary());

        for (auto iterator = oldFields->begin(); iterator != oldFields->end(); iterator.operator++())
        {
          PdfField *oldField = *iterator;
          PdfObject *oldFieldObj = &(oldField->GetObject());

          this->InnerRemoveField(oldFieldObj);

          renameFieldMap->insert(std::pair<PdfObject *, PdfField *>(oldFieldObj, oldField));
        }

        oldParentFieldDict->RemoveKey(PdfName("T"));
        oldParentFieldDict->AddKey(PdfName("T"), PdfString(lastFieldName));

        aggregateFieldObj = oldParentFieldObj;
      }

      PdfDictionary *aggregateFieldDict = &(aggregateFieldObj->GetDictionary());

      PdfObject *newParentFieldObj = nullptr;
      PdfArray *newParentFieldKids = nullptr;
      if (partialFieldName != "")
      {
        newParentFieldObj = this->InnerCreateFakeField(partialFieldName);

        PdfDictionary *newParentFieldDict = &(newParentFieldObj->GetDictionary());

        PdfObject *newParentFieldKidsObj = newParentFieldDict->FindKey(PdfName("Kids"));
        newParentFieldKids = &(newParentFieldKidsObj->GetArray());
      }
      else
      {
        PdfAcroForm *acroform = &(document->GetOrCreateAcroForm());
        PdfDictionary *acroformDict = &(acroform->GetDictionary());

        PdfObject *acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
        newParentFieldKids = &(acroformFieldsObj->GetArray());
      }

      if (aggregateFieldDict->HasKey(PdfName("Parent")))
      {
        aggregateFieldDict->RemoveKey(PdfName("Parent"));
      }

      if (newParentFieldObj != nullptr)
      {
        aggregateFieldDict->AddKeyIndirect(PdfName("Parent"), *newParentFieldObj);
      }

      newParentFieldKids->AddIndirect(*aggregateFieldObj);
    }
    else
    {
      PiPiManagedFields *newManagedFields = newFindIterator->second;
      std::set<PdfField *> *newFields = newManagedFields->AccessFields();

      if (newFields->size() == 1)
      {
        PdfField *newField = *(newFields->begin());
        PdfObject *newFieldObj = &(newField->GetObject());
        this->ExpandField(newFieldObj);
      }

      PdfField *newField = *(newFields->begin());
      PdfDictionary *newFieldDict = &(newField->GetDictionary());

      PdfObject *newParentFieldObj = newFieldDict->FindKey(PdfName("Parent"));
      PdfDictionary *newParentFieldDict = &(newParentFieldObj->GetDictionary());

      PdfObject *newParentFieldKidsObj = newParentFieldDict->FindKey(PdfName("Kids"));
      PdfArray *newParentFieldKids = &(newParentFieldKidsObj->GetArray());

      if (oldFields->size() == 1)
      {
        PdfField *oldField = *(oldFields->begin());
        PdfObject *oldFieldObj = &(oldField->GetObject());
        PdfDictionary *oldFieldDict = &(oldField->GetDictionary());

        this->InnerRemoveField(oldFieldObj);

        for (unsigned int i = 0; i < PiPiFieldConstants::SpecialHierarchicalFieldKeys.size(); i++)
        {
          PdfName SpecialHierarchicalFieldKey = PiPiFieldConstants::SpecialHierarchicalFieldKeys[i];

          if (oldFieldDict->HasKey(SpecialHierarchicalFieldKey))
          {
            oldFieldDict->RemoveKey(SpecialHierarchicalFieldKey);
          }
        }

        if (oldFieldDict->HasKey(PdfName("Parent")))
        {
          oldFieldDict->RemoveKey(PdfName("Parent"));
        }

        oldFieldDict->AddKeyIndirect(PdfName("Parent"), *newParentFieldObj);
        newParentFieldKids->AddIndirect(*oldFieldObj);

        renameFieldMap->insert(std::pair<PdfObject *, PdfField *>(oldFieldObj, oldField));
      }
      else
      {
        for (auto iterator = oldFields->begin(); iterator != oldFields->end(); iterator.operator++())
        {
          PdfField *oldField = *iterator;
          PdfObject *oldFieldObj = &(oldField->GetObject());
          PdfDictionary *oldFieldDict = &(oldField->GetDictionary());

          this->InnerRemoveField(oldFieldObj);

          oldFieldDict->AddKeyIndirect(PdfName("Parent"), *newParentFieldObj);
          newParentFieldKids->AddIndirect(*oldFieldDict);

          renameFieldMap->insert(std::pair<PdfObject *, PdfField *>(oldFieldObj, oldField));
        }
      }

      delete newFields;
    }

    delete oldFields;

    this->RenameFieldMap(renameFieldMap, newFieldName);

    delete renameFieldMap;

    this->RenameFieldAnnotation(oldFieldName, newFieldName);
  }

  std::map<const std::string, std::set<PdfAnnotation *> *> *PiPiFieldManager::SearchAllFieldAnnotation()
  {
    spdlog::trace("SearchAllFieldAnnotation");

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;
    std::map<const std::string, std::set<PdfAnnotation *> *> *outAnnotMap = new std::map<const std::string, std::set<PdfAnnotation *> *>();

    for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++())
    {
      const std::string fieldName = mapIterator->first;
      PiPiManagedAnnotations *managedAnnots = mapIterator->second;

      std::set<PdfAnnotation *> *annots = managedAnnots->AccessAnnotations();
      std::set<PdfAnnotation *> *outAnnots = new std::set<PdfAnnotation *>();

      outAnnots->insert(annots->begin(), annots->end());

      delete annots;

      outAnnotMap->insert(std::pair<const std::string, std::set<PdfAnnotation *> *>(fieldName, outAnnots));
    }

    return outAnnotMap;
  }

  std::set<PdfAnnotation *> *PiPiFieldManager::SearchFieldAnnotation(std::string fieldName)
  {
    spdlog::trace("SearchFieldAnnotation");

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;

    auto findIterator = annotMap->find(fieldName);
    if (findIterator == annotMap->end())
    {
      return new std::set<PdfAnnotation *>();
    }

    PiPiManagedAnnotations *managedAnnots = findIterator->second;
    std::set<PdfAnnotation *> *annots = managedAnnots->AccessAnnotations();
    std::set<PdfAnnotation *> *outAnnots = new std::set<PdfAnnotation *>();

    outAnnots->insert(annots->begin(), annots->end());

    delete annots;

    return outAnnots;
  }

  void PiPiFieldManager::RemoveAllFieldAnnotation()
  {
    spdlog::trace("RemoveAllFieldAnnotation");

    PdfDocument *document = this->document;
    PdfPageCollection *pages = &(document->GetPages());

    unsigned int pageCount = pages->GetCount();
    for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++)
    {
      PdfPage *page = &(pages->GetPageAt(pageIndex));
      PdfAnnotationCollection *annots = &(page->GetAnnotations());

      long annotCount = annots->GetCount();
      for (long annotIndex = annotCount - 1; annotIndex >= 0; annotIndex--)
      {
        annots->RemoveAnnotAt((unsigned int)annotIndex);
      }
    }

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;
    for (auto iterator = annotMap->begin(); iterator != annotMap->end(); iterator.operator++())
    {
      delete iterator->second;
      iterator->second = nullptr;
    }

    annotMap->clear();

    std::map<PdfObject *, PdfAnnotation *> *annotBridgeMap = this->annotBridgeMap;
    annotBridgeMap->clear();
  }

  void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName)
  {
    spdlog::trace("RemoveFieldAnnotation");

    this->RemoveFieldAnnotation(fieldName, -1);
  }

  void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName, long pageIndex)
  {
    spdlog::trace("RemoveFieldAnnotation");

    this->RemoveFieldAnnotation(fieldName, pageIndex, -1, -1);
  }

  void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y)
  {
    spdlog::trace("RemoveFieldAnnotation");

    this->RemoveFieldAnnotation(fieldName, pageIndex, x, y, -1, -1);
  }

  void PiPiFieldManager::RemoveFieldAnnotation(std::string fieldName, long pageIndex, double x, double y, double width, double height)
  {
    spdlog::trace("RemoveFieldAnnotation");

    PdfMemDocument *document = this->document;
    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;

    auto findIterator = annotMap->find(fieldName);
    if (findIterator == annotMap->end())
    {
      return;
    }

    PiPiManagedAnnotations *managedAnnots = findIterator->second;
    std::set<PdfAnnotation *> *annots = managedAnnots->AccessAnnotations();
    for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++())
    {
      PdfAnnotation *annot = *iterator;
      PdfObject *annotObj = &(annot->GetObject());

      PdfPage *page = &(annot->MustGetPage());

      int aPageIndex = PiPiPageUtil::SearchPageIndex(document, page);
      double ax = PiPiAnnotationUtil::ExtractAnnotationX(annot);
      double ay = PiPiAnnotationUtil::ExtractAnnotationY(annot);
      double aWidth = PiPiAnnotationUtil::ExtractAnnotationWidth(annot);
      double aHeight = PiPiAnnotationUtil::ExtractAnnotationHeight(annot);

      bool matched = (pageIndex == aPageIndex || pageIndex == -1) &&
                     (x == ax || x == -1) &&
                     (y == ay || y == -1) &&
                     (width == aWidth || width == -1) &&
                     (height == aHeight || height == -1);

      if (matched)
      {
        std::vector<unsigned int> *removePageAnnotIndexs = new std::vector<unsigned int>();

        PdfAnnotationCollection *pageAnnots = &(page->GetAnnotations());
        size_t pageAnnotCount = pageAnnots->GetCount();
        for (long pageAnnotIndex = pageAnnotCount - 1; pageAnnotIndex >= 0; pageAnnotIndex--)
        {
          PdfAnnotation *pageAnnot = &(pageAnnots->GetAnnotAt((unsigned int)pageAnnotIndex));
          if (&(pageAnnot->GetObject()) == annotObj)
          {
            removePageAnnotIndexs->push_back((unsigned int)pageAnnotIndex);
          }
        }

        size_t removePageAnnotIndexCount = removePageAnnotIndexs->size();
        for (unsigned int removePageAnnotIndexIndex = 0; removePageAnnotIndexIndex < removePageAnnotIndexCount; removePageAnnotIndexIndex++)
        {
          pageAnnots->RemoveAnnotAt((*removePageAnnotIndexs)[removePageAnnotIndexIndex]);
          this->RemoveAnnotationMap(annotObj, fieldName);
        }

        delete removePageAnnotIndexs;
      }
    }

    delete annots;
  }

  PdfAnnotation *PiPiFieldManager::CreateFieldAnnotation(std::string fieldName, unsigned int pageIndex, double x, double y, double width, double height)
  {
    spdlog::trace("CreateFieldAnnotation");

    PdfMemDocument *document = this->document;

    PdfPageCollection *pages = &(document->GetPages());
    size_t pageCount = pages->GetCount();
    if (pageIndex >= pageCount || pageIndex < 0)
    {
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPageIndex);
    }

    PdfPage *page = &(pages->GetPageAt(pageIndex));

    double pageHeight = PiPiPageUtil::ExtractPageHeight(page);

    PdfAnnotationCollection *annots = &(page->GetAnnotations());

    PdfAnnotation *annot = &(annots->CreateAnnot(PdfAnnotationType::Widget, Rect(x, pageHeight - y - height, width, height)));
    PdfObject *object = &(annot->GetObject());

    this->AddAnnotationMap(object, annot, fieldName);

    return annot;
  }

  void PiPiFieldManager::RenameFieldAnnotation(std::string oldFieldName, std::string newFieldName)
  {
    spdlog::trace("RenameFieldAnnotation");

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;

    auto findIterator = annotMap->find(oldFieldName);
    if (findIterator == annotMap->end())
    {
      return;
    }

    PiPiManagedAnnotations *managedAnnots = findIterator->second;

    std::set<PdfAnnotation *> *annots = managedAnnots->AccessAnnotations();

    std::map<PdfObject *, PdfAnnotation *> *renameAnnotMap = new std::map<PdfObject *, PdfAnnotation *>();
    for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++())
    {
      PdfAnnotation *annot = *iterator;
      PdfObject *annotObj = &(annot->GetObject());

      renameAnnotMap->insert(std::pair<PdfObject *, PdfAnnotation *>(annotObj, annot));
    }

    delete annots;

    this->RenameAnnotationMap(renameAnnotMap, newFieldName);

    delete renameAnnotMap;
  }

  PdfField *PiPiFieldManager::BridgeAnnotationToField(PdfAnnotation *annot)
  {
    spdlog::trace("BridgeAnnotationToField");

    std::map<PdfObject *, PdfField *> *fieldBridgeMap = this->fieldBridgeMap;
    PdfObject *object = &(annot->GetObject());

    return this->BridgeObjectToField(object);
  }

  PdfField *PiPiFieldManager::BridgeObjectToField(PdfObject *object)
  {
    spdlog::trace("BridgeObjectToField");

    auto findIterator = fieldBridgeMap->find(object);
    if (findIterator == fieldBridgeMap->end())
    {
      return nullptr;
    }

    return findIterator->second;
  }

  PdfAnnotation *PiPiFieldManager::BridgeFieldToAnnotation(PdfField *field)
  {
    spdlog::trace("BridgeFieldToAnnotation");

    std::map<PdfObject *, PdfAnnotation *> *annotBridgeMap = this->annotBridgeMap;
    PdfObject *object = &(field->GetObject());
    return this->BridgeObjectToAnnotation(object);
  }

  PdfAnnotation *PiPiFieldManager::BridgeObjectToAnnotation(PdfObject *object)
  {
    spdlog::trace("BridgeObjectToAnnotation");

    auto findIterator = annotBridgeMap->find(object);
    if (findIterator == annotBridgeMap->end())
    {
      return nullptr;
    }

    return findIterator->second;
  }

  void PiPiFieldManager::InitDocument(PdfMemDocument *document)
  {
    spdlog::trace("InitDocument");

    PiPiFieldCompatibilityUtil::Patch(document);

    this->document = document;
  }

  void PiPiFieldManager::InitFieldMap()
  {
    spdlog::trace("InitFieldMap");

    this->fieldMap = new std::map<const std::string, PiPiManagedFields *>();

    PdfMemDocument *document = this->document;

    PdfIndirectObjectList *indirectObjectList = &(document->GetObjects());

    PdfAcroForm *acroform = &(document->GetOrCreateAcroForm());
    PdfObject *acroformObj = &(acroform->GetObject());
    PdfDictionary *acroformDict = &(acroformObj->GetDictionary());

    PdfObject *fieldsObj = acroformDict->FindKey(PdfName("Fields"));
    if (fieldsObj == nullptr)
    {
      fieldsObj = &(indirectObjectList->CreateArrayObject());
      acroformDict->AddKeyIndirect(PdfName("Fields"), *fieldsObj);
    }

    PdfArray *fields = &(fieldsObj->GetArray());

    for (unsigned int idx = 0; idx < fields->size(); idx++)
    {
      PdfObject *fieldObj = &(fields->MustFindAt(idx));
      this->InnerSearchAllField(fieldObj);
    }
  }

  void PiPiFieldManager::InitAnnotationMap()
  {
    spdlog::trace("InitAnnotationMap");

    PdfDocument *document = this->document;

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = new std::map<const std::string, PiPiManagedAnnotations *>();

    PdfPageCollection *pages = &(document->GetPages());

    unsigned int pageCount = pages->GetCount();
    for (unsigned int pageIndex = 0; pageIndex < pageCount; pageIndex++)
    {
      PdfPage *page = &(pages->GetPageAt(pageIndex));

      PdfAnnotationCollection *annots = &(page->GetAnnotations());

      unsigned int annotCount = annots->GetCount();
      for (unsigned int annotIndex = 0; annotIndex < annotCount; annotIndex++)
      {
        PdfAnnotation *annot = &(annots->GetAnnotAt(annotIndex));

        const PdfObject &constObject = annot->GetObject();
        PdfObject &object = const_cast<PdfObject &>(constObject);

        std::unique_ptr<PdfField> fieldPtr;
        bool created = PdfField::TryCreateFromObject(object, fieldPtr);
        if (!created)
        {
          continue;
        }

        PdfField *field = fieldPtr.get();

        std::string name = field->GetFullName();

        auto findIterator = annotMap->find(name);
        if (findIterator == annotMap->end())
        {
          PiPiManagedAnnotations *managedAnnots = new PiPiManagedAnnotations(annot);
          annotMap->insert(std::pair<const std::string, PiPiManagedAnnotations *>(name, managedAnnots));
          continue;
        }

        PiPiManagedAnnotations *managedAnnots = findIterator->second;
        managedAnnots->ManageAnnotation(annot);
      }
    }

    this->annotMap = annotMap;
  }

  void PiPiFieldManager::InitFieldBridgeMap()
  {
    spdlog::trace("InitFieldBridgeMap");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;

    std::map<PdfObject *, PdfField *> *fieldBridgeMap = new std::map<PdfObject *, PdfField *>();

    for (auto mapIterator = fieldMap->begin(); mapIterator != fieldMap->end(); mapIterator.operator++())
    {
      PiPiManagedFields *managedFields = mapIterator->second;

      std::set<PdfField *> *fields = managedFields->AccessFields();

      for (auto iterator = fields->begin(); iterator != fields->end(); iterator.operator++())
      {
        PdfField *field = *iterator;
        PdfObject *object = &(field->GetObject());

        fieldBridgeMap->insert(std::pair<PdfObject *, PdfField *>(object, field));
      }

      delete fields;
    }

    this->fieldBridgeMap = fieldBridgeMap;
  }

  void PiPiFieldManager::InitAnnotationBridgeMap()
  {
    spdlog::trace("InitAnnotationBridgeMap");

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;

    std::map<PdfObject *, PdfAnnotation *> *annotBridgeMap = new std::map<PdfObject *, PdfAnnotation *>();

    for (auto mapIterator = annotMap->begin(); mapIterator != annotMap->end(); mapIterator.operator++())
    {
      PiPiManagedAnnotations *managedAnnots = mapIterator->second;

      std::set<PdfAnnotation *> *annots = managedAnnots->AccessAnnotations();

      for (auto iterator = annots->begin(); iterator != annots->end(); iterator.operator++())
      {
        PdfAnnotation *annot = *iterator;
        PdfObject *object = &(annot->GetObject());

        annotBridgeMap->insert(std::pair<PdfObject *, PdfAnnotation *>(object, annot));
      }

      delete annots;
    }

    this->annotBridgeMap = annotBridgeMap;
  }

  void PiPiFieldManager::InnerSearchAllField(PdfObject *fieldObj)
  {
    spdlog::trace("InnerSearchAllField");

    std::unique_ptr<PdfField> fieldPtr;
    bool created = PdfField::TryCreateFromObject(*fieldObj, fieldPtr);
    if (!created)
    {
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPdfFieldObject);
    }

    PdfField *field = fieldPtr.release();

    const std::string name = field->GetFullName();
    PdfFieldChildrenCollectionBase *childrens = &(field->GetChildren());

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;

    unsigned int childrenCount = childrens->GetCount();
    if (childrenCount == 0)
    {
      auto findIterator = fieldMap->find(name);
      if (findIterator == fieldMap->end())
      {
        PiPiManagedFields *manageFields = new PiPiManagedFields(field, true);
        fieldMap->insert(std::pair<const std::string, PiPiManagedFields *>(name, manageFields));
        return;
      }

      PiPiManagedFields *manageFields = findIterator->second;
      manageFields->ManageField(field);
      return;
    }

    bool allKidEnd = true;
    for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++)
    {
      PdfField *childrenField = &(childrens->GetFieldAt(childrenIndex));
      PdfObject *childrenObject = &(childrenField->GetObject());
      PdfDictionary *childrenDict = &(childrenObject->GetDictionary());

      if (childrenDict->HasKey(PdfName("T")))
      {
        allKidEnd = false;
        break;
      }
    }

    if (!allKidEnd)
    {
      PiPiManagedFields *manageFields = new PiPiManagedFields(field, false);
      fieldMap->insert(std::pair<const std::string, PiPiManagedFields *>(name, manageFields));
    }

    for (unsigned int childrenIndex = 0; childrenIndex < childrenCount; childrenIndex++)
    {
      PdfField *childrenField = &(childrens->GetFieldAt(childrenIndex));
      PdfObject *childrenObject = &(childrenField->GetObject());

      InnerSearchAllField(childrenObject);
    }
  }

  void PiPiFieldManager::InnerRemoveField(PdfObject *fieldObj)
  {
    spdlog::trace("InnerRemoveField");

    PdfDictionary *fieldDict = &(fieldObj->GetDictionary());

    PdfObject *parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
    if (parentFieldObj == nullptr)
    {
      this->InnerRemoveFieldFromAcroform(fieldObj);
    }
    else
    {
      this->InnerRemoveFieldFromParentField(fieldObj);
    }
  }

  void PiPiFieldManager::InnerRemoveFieldFromAcroform(PdfObject *fieldObj)
  {
    spdlog::trace("InnerRemoveFieldFromAcroform");

    PdfField *field = this->BridgeObjectToField(fieldObj);

    PdfAcroForm *acroform = &(document->GetOrCreateAcroForm());
    PdfDictionary *acroformDict = &(acroform->GetDictionary());

    PdfObject *acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
    PdfArray *acroformFields = &(acroformFieldsObj->GetArray());

    size_t acroformFieldCount = acroformFields->size();
    unsigned int pos = 0;
    for (unsigned int idx = 0; idx < acroformFieldCount; idx++)
    {
      if (fieldObj == &(acroformFields->MustFindAt(idx)))
      {
        acroformFields->RemoveAt(idx);
        break;
      }
    }

    std::string fieldName = field->GetFullName();
    this->RemoveFieldMap(fieldObj, fieldName);
  }

  void PiPiFieldManager::InnerRemoveFieldFromParentField(PdfObject *fieldObj)
  {
    spdlog::trace("InnerRemoveFieldFromParentField");

    PdfField *field = this->BridgeObjectToField(fieldObj);
    PdfDictionary *fieldDict = &(fieldObj->GetDictionary());

    PdfObject *parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
    PdfDictionary *parentFieldDict = &(parentFieldObj->GetDictionary());

    PdfObject *parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
    PdfArray *parentFieldKids = &(parentFieldKidsObj->GetArray());

    size_t parentFieldKidCount = parentFieldKids->size();
    for (unsigned int idx = 0; idx < parentFieldKidCount; idx++)
    {
      if (fieldObj == &(parentFieldKids->MustFindAt(idx)))
      {
        parentFieldKids->RemoveAt(idx);
        break;
      }
    }

    std::string fieldName = field->GetFullName();
    this->RemoveFieldMap(fieldObj, fieldName);

    if (parentFieldKidCount - 1 == 1)
    {
      PdfObject *brotherFieldObj = &(parentFieldKids->MustFindAt(0));
      PdfDictionary *brotherFieldDict = &(brotherFieldObj->GetDictionary());

      if (!brotherFieldDict->HasKey(PdfName("T")))
      {
        this->RestrictField(brotherFieldObj);
      }
    }

    if (parentFieldKidCount - 1 == 0)
    {
      this->InnerRemoveField(parentFieldObj);
    }
  }

  PdfObject *PiPiFieldManager::InnerCreateFakeField(std::string fieldName)
  {
    spdlog::trace("InnerCreateFakeField");

    PdfMemDocument *document = this->document;

    PdfField *cField = nullptr;

    std::vector<std::string> *splits = PiPiStringCommon::split(fieldName, ".");
    std::vector<std::string> *createSplits = new std::vector<std::string>();

    while (cField == nullptr && splits->size())
    {
      std::string partialFieldName = PiPiStringCommon::join(splits, ".");

      auto findIterator = fieldMap->find(partialFieldName);
      if (findIterator != fieldMap->end())
      {
        PiPiManagedFields *managedFields = findIterator->second;

        std::set<PdfField *> *fields = managedFields->AccessFields();

        cField = *(fields->begin());

        delete fields;

        break;
      }

      std::string lastPartialFieldName = splits->back();

      createSplits->push_back(lastPartialFieldName);
      splits->pop_back();
    }

    delete splits;

    PdfIndirectObjectList *indirectObjectList = &(document->GetObjects());

    if (cField == nullptr)
    {
      std::string firstPartialFieldName = createSplits->back();

      PdfAcroForm *acroform = &(document->GetOrCreateAcroForm());
      PdfDictionary *acroformDict = &(acroform->GetDictionary());

      PdfObject *acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
      PdfArray *acroformFields = &(acroformFieldsObj->GetArray());

      PdfObject *fieldObj = &(indirectObjectList->CreateDictionaryObject());
      PdfDictionary *fieldDict = &(fieldObj->GetDictionary());

      fieldDict->AddKey(PdfName("Kids"), PdfArray());
      fieldDict->AddKey(PdfName("T"), PdfString(firstPartialFieldName));
      acroformFields->AddIndirect(*fieldObj);

      std::unique_ptr<PdfField> fieldPtr;
      PdfField::TryCreateFromObject(*fieldObj, fieldPtr);
      PdfField *field = fieldPtr.release();

      std::string fieldName = field->GetFullName();

      this->AddFieldMap(fieldObj, field, fieldName, false);

      cField = field;

      createSplits->pop_back();
    }

    std::reverse(createSplits->begin(), createSplits->end());

    for (auto iterator = createSplits->begin(); iterator != createSplits->end(); iterator.operator++())
    {
      std::string partialFieldName = *iterator;

      PdfObject *cFieldObj = &(cField->GetObject());
      PdfDictionary *cFieldDict = &(cField->GetDictionary());

      PdfObject *cFieldKidsObj = cFieldDict->FindKey(PdfName("Kids"));
      PdfArray *cFieldKids = &(cFieldKidsObj->GetArray());

      PdfObject *fieldObj = &(indirectObjectList->CreateDictionaryObject());
      PdfDictionary *fieldDict = &(fieldObj->GetDictionary());

      fieldDict->AddKey(PdfName("Kids"), PdfArray());
      fieldDict->AddKey(PdfName("T"), PdfString(partialFieldName));
      fieldDict->AddKeyIndirect(PdfName("Parent"), *cFieldObj);

      cFieldKids->AddIndirect(*fieldObj);

      std::unique_ptr<PdfField> fieldPtr;
      PdfField::TryCreateFromObject(*fieldObj, fieldPtr);
      PdfField *field = fieldPtr.release();

      std::string fieldName = field->GetFullName();

      this->AddFieldMap(fieldObj, field, fieldName, false);

      cField = field;
    }

    delete createSplits;

    PdfObject *cFieldObj = &(cField->GetObject());

    return cFieldObj;
  }

  PdfObject *PiPiFieldManager::InnerCreateRealField(std::string fieldName, PiPiFieldType type, unsigned int pageIndex, double x, double y, double width, double height)
  {
    spdlog::trace("InnerCreateRealField");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;

    PdfAnnotation *annot = this->CreateFieldAnnotation(fieldName, pageIndex, x, y, width, height);
    PdfObject *object = &(annot->GetObject());
    PdfDictionary *dict = &(object->GetDictionary());

    switch (type)
    {
    case PiPiFieldType::TextBox:
      dict->AddKey(PdfName("FT"), PdfName("Tx"));
      break;
    case PiPiFieldType::CheckBox:
      dict->AddKey(PdfName("FT"), PdfName("Btn"));
      break;
    case PiPiFieldType::Unknown:
    default:
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidPdfFieldType);
    }

    // 已有欄位 -> 同名書籤 -> 從兄弟找爸爸
    // 未有書籤 -> 全新書籤 -> 建立假爸爸
    auto findIterator = fieldMap->find(fieldName);
    if (findIterator != fieldMap->end())
    {
      PiPiManagedFields *managedFields = findIterator->second;

      std::set<PdfField *> *brotherFields = managedFields->AccessFields();

      PdfField *brotherField = *(brotherFields->begin());
      PdfObject *brotherFieldObj = &(brotherField->GetObject());
      PdfDictionary *brotherFieldDict = &(brotherFieldObj->GetDictionary());

      delete brotherFields;

      PdfObject *parentObject = brotherFieldDict->FindKey(PdfName("Parent"));
      PdfDictionary *parentDict = &(parentObject->GetDictionary());

      PdfObject *parentKidsObject = parentDict->FindKey(PdfName("Kids"));
      PdfArray *parentKids = &(parentKidsObject->GetArray());

      dict->AddKeyIndirect(PdfName("Parent"), *parentObject);
      parentKids->AddIndirect(*object);
    }
    else
    {
      std::vector<std::string> *splits = PiPiStringCommon::split(fieldName, ".");

      std::string lastFieldName = splits->back();

      splits->pop_back();
      std::string parentFieldName = PiPiStringCommon::join(splits, ".");

      delete splits;

      PdfObject *parentObject = this->InnerCreateFakeField(parentFieldName);
      PdfDictionary *parentDict = &(parentObject->GetDictionary());

      PdfObject *parentKidsObject = parentDict->FindKey(PdfName("Kids"));
      PdfArray *parentKids = &(parentKidsObject->GetArray());

      dict->AddKeyIndirect(PdfName("Parent"), *parentObject);
      dict->AddKey(PdfName("T"), PdfString(lastFieldName));

      parentKids->AddIndirect(*object);
    }

    std::unique_ptr<PdfField> fieldPtr;
    PdfField::TryCreateFromObject(*object, fieldPtr);
    PdfField *field = fieldPtr.release();

    this->AddFieldMap(object, field, fieldName, true);
  }

  void PiPiFieldManager::RestrictField(PdfObject *fieldObj)
  {
    spdlog::trace("RestrictField");

    PdfMemDocument *document = this->document;

    PdfDictionary *fieldDict = &(fieldObj->GetDictionary());

    // 取得爸爸
    PdfObject *parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
    if (parentFieldObj == nullptr)
    {
      throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidRestrictFieldOperation);
    }

    PdfDictionary *parentFieldDict = &(parentFieldObj->GetDictionary());

    // 處理父子間關係
    fieldDict->RemoveKey(PdfName("Parent"));
    parentFieldDict->RemoveKey(PdfName("Kids"));

    for (auto iterator = parentFieldDict->begin(); iterator != parentFieldDict->end(); iterator.operator++())
    {
      const PdfName name = iterator->first;
      PdfObject *object = &(iterator->second);

      if (fieldDict->HasKey(name))
      {
        continue;
      }

      fieldDict->AddKey(name, *object);
    }

    // 取得爺爺
    PdfObject *grandFieldObj = parentFieldDict->FindKey(PdfName("Parent"));
    PdfArray *grandFieldKids = nullptr;

    if (grandFieldObj == nullptr)
    {
      PdfAcroForm *acroform = document->GetAcroForm();
      PdfDictionary *acroformDict = &(acroform->GetDictionary());
      PdfObject *acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
      grandFieldKids = &(acroformFieldsObj->GetArray());
    }
    else
    {
      PdfDictionary *grandFieldDict = &(grandFieldObj->GetDictionary());
      PdfObject *grandKidsObj = grandFieldDict->FindKey(PdfName("Kids"));
      grandFieldKids = &(grandKidsObj->GetArray());
    }

    // 處理爺爸間關係
    for (unsigned int index = 0; index < grandFieldKids->size(); index++)
    {
      PdfObject *uncleFieldObj = grandFieldKids->FindAt(index);
      if (uncleFieldObj == parentFieldObj)
      {
        grandFieldKids->RemoveAt(index);
        break;
      }
    }

    // 處理爺孫間關係
    if (grandFieldObj != nullptr)
    {
      fieldDict->AddKeyIndirect(PdfName("Parent"), *grandFieldObj);
    }

    grandFieldKids->AddIndirect(*fieldObj);
  }

  void PiPiFieldManager::ExpandField(PdfObject *fieldObj)
  {
    spdlog::trace("ExpandField");

    PdfMemDocument *document = this->document;

    PdfIndirectObjectList *indirectObjectList = &(document->GetObjects());

    PdfDictionary *fieldDict = &(fieldObj->GetDictionary());

    PdfObject *expandFieldObj = &(indirectObjectList->CreateDictionaryObject());
    PdfDictionary *expandFieldDict = &(expandFieldObj->GetDictionary());

    PdfObject *parentFieldObj = fieldDict->FindKey(PdfName("Parent"));

    if (parentFieldObj != nullptr)
    {
      fieldDict->RemoveKey(PdfName("Parent"));
    }

    for (unsigned int i = 0; i < PiPiFieldConstants::SpecialHierarchicalFieldKeys.size(); i++)
    {
      PdfName SpecialHierarchicalFieldKey = PiPiFieldConstants::SpecialHierarchicalFieldKeys[i];

      PdfObject *fieldValueObj = fieldDict->FindKey(SpecialHierarchicalFieldKey);
      if (fieldValueObj == nullptr)
      {
        continue;
      }

      expandFieldDict->AddKey(SpecialHierarchicalFieldKey, *fieldValueObj);
      fieldDict->RemoveKey(SpecialHierarchicalFieldKey);
    }

    if (parentFieldObj == nullptr)
    {
      PdfAcroForm *acroform = document->GetAcroForm();
      PdfDictionary *acroformDict = &(acroform->GetDictionary());
      PdfObject *acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
      PdfArray *acroformFields = &(acroformFieldsObj->GetArray());

      for (unsigned int idx = 0; idx < acroformFields->size(); idx++)
      {
        PdfObject *acroformFieldObj = acroformFields->FindAt(idx);
        if (acroformFieldObj == fieldObj)
        {
          acroformFields->RemoveAt(idx);
          break;
        }
      }

      acroformFields->AddIndirect(*expandFieldObj);
    }
    else
    {
      PdfDictionary *parentFieldDict = &(parentFieldObj->GetDictionary());
      PdfObject *parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
      PdfArray *parentFieldKids = &(parentFieldKidsObj->GetArray());

      for (unsigned int idx = 0; idx < parentFieldKids->size(); idx++)
      {
        PdfObject *parentFieldKidObj = parentFieldKids->FindAt(idx);
        if (parentFieldKidObj == fieldObj)
        {
          parentFieldKids->RemoveAt(idx);
          break;
        }
      }

      expandFieldDict->AddKeyIndirect(PdfName("Parent"), *parentFieldObj);
      parentFieldKids->AddIndirect(*expandFieldObj);
    }

    expandFieldDict->AddKey(PdfName("Kids"), PdfArray());
    PdfObject *expandFieldKidsObj = expandFieldDict->FindKey(PdfName("Kids"));
    PdfArray *expandFieldKids = &(expandFieldKidsObj->GetArray());

    fieldDict->AddKeyIndirect(PdfName("Parent"), *expandFieldObj);
    expandFieldKids->AddIndirect(*fieldObj);
  }

  void PiPiFieldManager::RemoveFieldMap(PdfObject *fieldObj, std::string fieldName)
  {
    spdlog::trace("RemoveFieldMap");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;
    std::map<PdfObject *, PdfField *> *fieldBridgeMap = this->fieldBridgeMap;

    PdfField *field = this->BridgeObjectToField(fieldObj);
    auto findFieldMapIterator = fieldMap->find(fieldName);
    if (findFieldMapIterator != fieldMap->end())
    {
      PiPiManagedFields *managedFields = findFieldMapIterator->second;
      managedFields->UnManageField(field);

      if (managedFields->IsEmpty())
      {
        fieldMap->erase(findFieldMapIterator);
      }
    }

    auto findFieldBridgeMapIterator = fieldBridgeMap->find(fieldObj);
    if (findFieldBridgeMapIterator != fieldBridgeMap->end())
    {
      fieldBridgeMap->erase(findFieldBridgeMapIterator);
    }
  }

  void PiPiFieldManager::RemoveAnnotationMap(PdfObject *annotObj, std::string fieldName)
  {
    spdlog::trace("RemoveAnnotationMap");

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;
    std::map<PdfObject *, PdfAnnotation *> *annotBridgeMap = this->annotBridgeMap;

    PdfAnnotation *annot = this->BridgeObjectToAnnotation(annotObj);

    auto findAnnotMapIterator = annotMap->find(fieldName);
    if (findAnnotMapIterator != annotMap->end())
    {
      PiPiManagedAnnotations *managedAnnots = findAnnotMapIterator->second;
      managedAnnots->UnManageAnnotation(annot);

      if (managedAnnots->IsEmpty())
      {
        annotMap->erase(findAnnotMapIterator);
      }
    }

    auto findAnnotBridgeMapIterator = annotBridgeMap->find(annotObj);
    if (findAnnotBridgeMapIterator != annotBridgeMap->end())
    {
      annotBridgeMap->erase(findAnnotBridgeMapIterator);
    }
  }

  void PiPiFieldManager::AddFieldMap(PdfObject *fieldObj, PdfField *field, std::string fieldName, bool real)
  {
    spdlog::trace("AddFieldMap");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;
    std::map<PdfObject *, PdfField *> *fieldBridgeMap = this->fieldBridgeMap;

    auto findFieldMapIterator = fieldMap->find(fieldName);
    if (findFieldMapIterator != fieldMap->end())
    {
      PiPiManagedFields *managedFields = findFieldMapIterator->second;

      if (managedFields->IsReal() != real)
      {
        throw PiPiManageFieldException(PiPiManageFieldException::PiPiManageFieldExceptionCode::InvalidRealStatus);
      }

      managedFields->ManageField(field);
    }
    else
    {
      PiPiManagedFields *managedFields = new PiPiManagedFields(field, real);
      fieldMap->insert(std::pair<const std::string, PiPiManagedFields *>(fieldName, managedFields));
    }

    fieldBridgeMap->insert(std::pair<PdfObject *, PdfField *>(fieldObj, field));
  }

  void PiPiFieldManager::AddAnnotationMap(PdfObject *annotObj, PdfAnnotation *annot, std::string fieldName)
  {
    spdlog::trace("AddAnnotationMap");

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;
    std::map<PdfObject *, PdfAnnotation *> *annotBridgeMap = this->annotBridgeMap;

    auto findAnnotMapIterator = annotMap->find(fieldName);
    if (findAnnotMapIterator != annotMap->end())
    {
      PiPiManagedAnnotations *managedAnnots = findAnnotMapIterator->second;
      managedAnnots->ManageAnnotation(annot);
    }
    else
    {
      PiPiManagedAnnotations *managedAnnots = new PiPiManagedAnnotations(annot);
      annotMap->insert(std::pair<const std::string, PiPiManagedAnnotations *>(fieldName, managedAnnots));
    }

    annotBridgeMap->insert(std::pair<PdfObject *, PdfAnnotation *>(annotObj, annot));
  }

  void PiPiFieldManager::RenameFieldMap(std::map<PdfObject *, PdfField *> *renameFieldMap, std::string fieldName)
  {
    spdlog::trace("RenameFieldMap");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;
    std::map<PdfObject *, PdfField *> *fieldBridgeMap = this->fieldBridgeMap;

    std::set<PdfField *> *renameFields = new std::set<PdfField *>();
    for (auto iterator = renameFieldMap->begin(); iterator != renameFieldMap->end(); iterator.operator++())
    {
      PdfField *renameField = iterator->second;
      renameFields->insert(renameField);
    }

    auto findIterator = fieldMap->find(fieldName);
    if (findIterator == fieldMap->end())
    {
      PiPiManagedFields *managedFields = new PiPiManagedFields(renameFields);
      fieldMap->insert(std::pair<const std::string, PiPiManagedFields *>(fieldName, managedFields));
    }
    else
    {
      PiPiManagedFields *managedFields = findIterator->second;
      for (auto iterator = renameFields->begin(); iterator != renameFields->end(); iterator.operator++())
      {
        PdfField *renameField = *iterator;
        managedFields->ManageField(renameField);
      }
    }

    delete renameFields;

    fieldBridgeMap->insert(renameFieldMap->begin(), renameFieldMap->end());
  }

  void PiPiFieldManager::RenameAnnotationMap(std::map<PdfObject *, PdfAnnotation *> *renameAnnotMap, std::string fieldName)
  {
    spdlog::trace("RenameAnnotationMap");

    std::map<const std::string, PiPiManagedAnnotations *> *annotMap = this->annotMap;
    std::map<PdfObject *, PdfAnnotation *> *annotBridgeMap = this->annotBridgeMap;

    std::set<PdfAnnotation *> *renameAnnots = new std::set<PdfAnnotation *>();
    for (auto iterator = renameAnnotMap->begin(); iterator != renameAnnotMap->end(); iterator.operator++())
    {
      PdfAnnotation *renameAnnot = iterator->second;
      renameAnnots->insert(renameAnnot);
    }

    auto findIterator = annotMap->find(fieldName);
    if (findIterator == annotMap->end())
    {
      PiPiManagedAnnotations *managedAnnots = new PiPiManagedAnnotations(renameAnnots);
      annotMap->insert(std::pair<const std::string, PiPiManagedAnnotations *>(fieldName, managedAnnots));
    }
    else
    {
      PiPiManagedAnnotations *managedAnnots = findIterator->second;
      for (auto iterator = renameAnnots->begin(); iterator != renameAnnots->end(); iterator.operator++())
      {
        PdfAnnotation *renameAnnot = *iterator;
        managedAnnots->ManageAnnotation(renameAnnot);
      }
    }

    delete renameAnnots;

    annotBridgeMap->insert(renameAnnotMap->begin(), renameAnnotMap->end());
  }

  bool PiPiFieldManager::IsDuplicateFieldExists(std::string fieldName)
  {
    spdlog::trace("IsDuplicateFieldExists");

    std::map<const std::string, PiPiManagedFields *> *fieldMap = this->fieldMap;

    for (auto iterator = fieldMap->begin(); iterator != fieldMap->end(); iterator.operator++())
    {
      const std::string name = iterator->first;

      if (PiPiStringCommon::startsWith(name, fieldName) && name != fieldName)
      {
        return true;
      }
    }

    return false;
  }
}
