#include "PiPiSameFieldUtil.h"

namespace PiPi
{
  void PiPiSameFieldUtil::ExpandField(PdfMemDocument* document, PdfObject* fieldObj)
  {
    spdlog::trace("ExpandField");

    PdfIndirectObjectList* indirectObjectList = &(document->GetObjects());

    PdfDictionary* fieldDict = &(fieldObj->GetDictionary());

    PdfObject* expandFieldObj = &(indirectObjectList->CreateDictionaryObject());
    PdfDictionary* expandFieldDict = &(expandFieldObj->GetDictionary());

    PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));

    if (parentFieldObj != nullptr)
    {
      fieldDict->RemoveKey(PdfName("Parent"));
    }

    for (unsigned int i = 0; i < PiPiFieldConstants::SpecialHierarchicalFieldKeys.size(); i++)
    {
      PdfName SpecialHierarchicalFieldKey = PiPiFieldConstants::SpecialHierarchicalFieldKeys[i];

      PdfObject* fieldValueObj = fieldDict->FindKey(SpecialHierarchicalFieldKey);
      if (fieldValueObj == nullptr)
      {
        continue;
      }

      expandFieldDict->AddKey(SpecialHierarchicalFieldKey, *fieldValueObj);
      fieldDict->RemoveKey(SpecialHierarchicalFieldKey);
    }

    if (parentFieldObj == nullptr)
    {
      PdfAcroForm* acroform = document->GetAcroForm();
      PdfDictionary* acroformDict = &(acroform->GetDictionary());
      PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
      PdfArray* acroformFields = &(acroformFieldsObj->GetArray());

      for (unsigned int idx = 0; idx < acroformFields->size(); idx++)
      {
        PdfObject* acroformFieldObj = acroformFields->FindAt(idx);
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
      PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());
      PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
      PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());

      for (unsigned int idx = 0; idx < parentFieldKids->size(); idx++)
      {
        PdfObject* parentFieldKidObj = parentFieldKids->FindAt(idx);
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
    PdfObject* expandFieldKidsObj = expandFieldDict->FindKey(PdfName("Kids"));
    PdfArray* expandFieldKids = &(expandFieldKidsObj->GetArray());

    fieldDict->AddKeyIndirect(PdfName("Parent"), *expandFieldObj);
    expandFieldKids->AddIndirect(*fieldObj);
  }

	void PiPiSameFieldUtil::RestrictField(PdfMemDocument* document, PdfObject* fieldObj)
  {
    spdlog::trace("RestrictField");

    PdfDictionary* fieldDict = &(fieldObj->GetDictionary());

    PdfObject* parentFieldObj = fieldDict->FindKey(PdfName("Parent"));
    PdfDictionary* parentFieldDict = &(parentFieldObj->GetDictionary());

    fieldDict->RemoveKey(PdfName("Parent"));

    for (auto iterator = parentFieldDict->begin(); iterator != parentFieldDict->end(); iterator.operator++())
    {
      const PdfName key = iterator->first;
      PdfObject* value = &(iterator->second);

      if (key == PdfName("Kids"))
      {
        continue;
      }

      if (fieldDict->HasKey(key))
      {
        continue;
      }

      fieldDict->AddKey(key, *value);
    }

    PdfObject* parentFieldKidsObj = parentFieldDict->FindKey(PdfName("Kids"));
    PdfArray* parentFieldKids = &(parentFieldKidsObj->GetArray());

    PdfObject* grandFieldObj = parentFieldDict->FindKey(PdfName("Parent"));
    if (grandFieldObj != nullptr)
    {
      PdfDictionary* grandFieldDict = &(grandFieldObj->GetDictionary());

      PdfObject* grandFieldKidsObj = grandFieldDict->FindKey(PdfName("Kids"));
      PdfArray* grandFieldKids = &(grandFieldKidsObj->GetArray());

      for (unsigned int idx = 0; idx < grandFieldKids->size(); idx++)
      {
        PdfObject* grandFieldKidObj = &(grandFieldKids->MustFindAt(idx));
        if (grandFieldKidObj == parentFieldObj)
        {
          grandFieldKids->RemoveAt(idx);
          break;
        }
      }

      grandFieldKids->AddIndirect(*fieldObj);
    }
    else
    {
      PdfAcroForm* acroform = document->GetAcroForm();
      PdfDictionary* acroformDict = &(acroform->GetDictionary());

      PdfObject* acroformFieldsObj = acroformDict->FindKey(PdfName("Fields"));
      PdfArray* acroformFields = &(acroformFieldsObj->GetArray());

      for (unsigned int idx = 0; idx < acroformFields->size(); idx++)
      {
        PdfObject* acroformFieldObj = &(acroformFields->MustFindAt(idx));
        if (acroformFieldObj == parentFieldObj)
        {
          acroformFields->RemoveAt(idx);
          break;
        }
      }

      acroformFields->AddIndirect(*fieldObj);
    }
	}
}
