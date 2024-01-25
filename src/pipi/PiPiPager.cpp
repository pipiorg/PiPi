#include "PiPiPager.h"

namespace PiPi
{
  PiPiPager::PiPiPager(std::vector<std::tuple<PdfMemDocument *, PiPiOperator *>> *docs)
  {
    this->operable = true;
    this->docs = docs;
  }

  bool PiPiPager::IsOperable()
  {
    spdlog::trace("IsOperable");

    return this->operable;
  }

  void PiPiPager::Merge(std::vector<size_t> *indexs, std::vector<char> **newPdf)
  {
    spdlog::trace("Merge");

    if (!this->IsOperable())
    {
      throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InOperable);
    }

    std::vector<std::tuple<PdfMemDocument *, PiPiOperator *>> *docs = this->docs;

    PdfMemDocument *mergedDoc = new PdfMemDocument();

    for (size_t i = 0; i < indexs->size(); i++)
    {
      size_t index = indexs->at(i);

      if (index >= docs->size())
      {
        throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::IndexOutOfRange);
      }

      PiPiOperator *op;
      PdfMemDocument *doc;

      std::tie(doc, op) = (*docs)[index];

      PdfPageCollection *pages = &(doc->GetPages());
      this->CopyPage(mergedDoc, doc, 0, pages->GetCount());
    }

    *newPdf = new vector<char>();
    PoDoFo::VectorStreamDevice outputStreamDevice(**newPdf);

    mergedDoc->Save(outputStreamDevice);

    delete mergedDoc;
  }

  void PiPiPager::Split(size_t index, std::string instruction, std::vector<std::vector<char> *> **newPdfs)
  {
    spdlog::trace("Split");

    if (!this->IsOperable())
    {
      throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InOperable);
    }

    std::vector<std::tuple<PdfMemDocument *, PiPiOperator *>> *docs = this->docs;

    if (index >= docs->size())
    {
      throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::IndexOutOfRange);
    }

    PiPiOperator *op;
    PdfMemDocument *doc;

    std::tie(doc, op) = (*docs)[index];

    std::vector<vector<char> *> *dAllNewPdfBytes = new std::vector<vector<char> *>();

    std::vector<std::tuple<size_t, size_t>> *instructionPairs = this->ParseSplitInstruction(instruction);
    for (auto iterator = instructionPairs->begin(); iterator != instructionPairs->end(); iterator.operator++())
    {
      PdfMemDocument *splitedDoc = new PdfMemDocument();
      PdfPageCollection *splitedPages = &(splitedDoc->GetPages());

      size_t s;
      size_t e;

      std::tie(s, e) = *iterator;

      this->CopyPage(splitedDoc, doc, (unsigned int)s, (unsigned int)e);

      vector<char> *outputVector = new vector<char>();
      PoDoFo::VectorStreamDevice outputStreamDevice(*outputVector);
      splitedDoc->Save(outputStreamDevice);

      dAllNewPdfBytes->push_back(outputVector);

      delete splitedDoc;
    }

    delete instructionPairs;

    *newPdfs = new std::vector<std::vector<char> *>();

    for (size_t i = 0; i < dAllNewPdfBytes->size(); i++)
    {
      vector<char> *dNewPdfBytes = dAllNewPdfBytes->at(i);
      (*newPdfs)->push_back(dNewPdfBytes);
    }

    delete dAllNewPdfBytes;
  }

  std::vector<std::tuple<size_t, size_t>> *PiPiPager::ParseSplitInstruction(std::string instruction)
  {
    spdlog::trace("ParseSplitInstruction");

    std::vector<std::tuple<size_t, size_t>> *instructionPairs = new std::vector<std::tuple<size_t, size_t>>();

    std::vector<std::string> *docInstructions = PiPiStringCommon::split(instruction, ",");

    for (auto iterator = docInstructions->begin(); iterator != docInstructions->end(); iterator.operator++())
    {
      std::string docInstruction = *iterator;

      std::vector<std::string> *pageInstructions = PiPiStringCommon::split(docInstruction, "-");

      if (pageInstructions->size() == 1)
      {
        std::string one = (*pageInstructions)[0];

        int oneNum = -1;

        try
        {
          oneNum = std::stoi(one);
        }
        catch (std::invalid_argument &e)
        {
          throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
        }
        catch (std::out_of_range &e)
        {
          throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
        }

        if (oneNum < 1)
        {
          throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
        }

        instructionPairs->push_back(std::make_tuple(oneNum - 1, oneNum));
      }
      else if (pageInstructions->size() == 2)
      {
        std::string one = (*pageInstructions)[0];
        std::string two = (*pageInstructions)[1];

        int oneNum = -1;
        int twoNum = -1;

        try
        {
          oneNum = std::stoi(one);
          twoNum = std::stoi(two);
        }
        catch (std::invalid_argument &e)
        {
          throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
        }
        catch (std::out_of_range &e)
        {
          throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
        }

        if (oneNum < 1 || twoNum < 1)
        {
          throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
        }

        if (oneNum == twoNum)
        {
          instructionPairs->push_back(std::make_tuple(oneNum - 1, oneNum));
        }
        else if (oneNum > twoNum)
        {
          instructionPairs->push_back(std::make_tuple(twoNum - 1, oneNum - 1));
        }
        else
        {
          instructionPairs->push_back(std::make_tuple(oneNum - 1, twoNum - 1));
        }
      }
      else
      {
        throw PiPiPageException(PiPiPageException::PiPiPageExceptionCode::InvalidSplitInstruction);
      }

      delete pageInstructions;
    }

    delete docInstructions;

    return instructionPairs;
  }

  void PiPiPager::CopyPage(PdfMemDocument *to, PdfMemDocument *from, unsigned int start, unsigned int end)
  {
    spdlog::trace("CopyPage");

    unsigned int base = to->GetPages().GetCount();

    this->CopyPageDocument(to, from, start, end);
    this->CopyPageAcroform(to, from, base, start, end);
  }

  void PiPiPager::CopyPageDocument(PdfMemDocument *to, PdfMemDocument *from, unsigned int start, unsigned int end)
  {
    spdlog::trace("CopyPageDocument");

    PdfPageCollection *toPages = &(to->GetPages());
    toPages->AppendDocumentPages(*from, start, end - start);
  }

  void PiPiPager::CopyPageAcroform(PdfMemDocument *to, PdfMemDocument *from, unsigned int base, unsigned int start, unsigned int end)
  {
    spdlog::trace("CopyPageAcroform");

    this->CopyPageAcroformField(to, from, base, start, end);
    this->CopyPageAcroformFont(to, from, base, start, end);
  }

  void PiPiPager::CopyPageAcroformField(PdfMemDocument *to, PdfMemDocument *from, unsigned int base, unsigned int start, unsigned int end)
  {
    spdlog::trace("CopyPageAcroformField");

    PdfAcroForm *toAcroform = &(to->GetOrCreateAcroForm());
    PdfDictionary *toAcroformDict = &(toAcroform->GetDictionary());

    PdfObject *toAcroformFieldsObj = toAcroformDict->FindKey(PdfName("Fields"));
    if (toAcroformFieldsObj == nullptr)
    {
      toAcroformDict->AddKey(PdfName("Fields"), PdfArray());
      toAcroformFieldsObj = toAcroformDict->FindKey(PdfName("Fields"));
    }

    PdfArray *toAcroformFields = &(toAcroformFieldsObj->GetArray());

    PdfPageCollection *toPages = &(to->GetPages());
    for (unsigned int i = base; i < base + (end - start); i++)
    {
      PdfPage *toPage = &(toPages->GetPageAt(i));
      PdfAnnotationCollection *toAnnots = &(toPage->GetAnnotations());
      for (unsigned int j = 0; j < toAnnots->GetCount(); j++)
      {
        PdfAnnotation *toAnnot = &(toAnnots->GetAnnotAt(j));
        PdfObject *toObj = &(toAnnot->GetObject());
        PdfDictionary *toDict = &(toObj->GetDictionary());

        std::unique_ptr<PdfField> toFieldPtr;
        bool created = PdfField::TryCreateFromObject(*toObj, toFieldPtr);
        if (!created)
        {
          continue;
        }

        std::vector<PdfObject *> *toLevelFeilds = new std::vector<PdfObject *>();

        PdfObject *toRootObj = toObj;
        PdfObject *toParentObj = toDict->FindKey(PdfName("Parent"));
        toLevelFeilds->push_back(toRootObj);

        while (toParentObj != nullptr)
        {
          toRootObj = toParentObj;
          toParentObj = (&(toRootObj->GetDictionary()))->FindKey(PdfName("Parent"));
          toLevelFeilds->push_back(toRootObj);
        }

        std::reverse(toLevelFeilds->begin(), toLevelFeilds->end());

        PdfObject *toLevelRootFieldObj = (*toLevelFeilds)[0];
        PdfObject *toExistsLevelRootFieldObj = nullptr;
        for (size_t idx = 0; idx < toAcroformFields->size(); idx++)
        {
          PdfObject *toAcroformFieldObj = toAcroformFields->FindAt((unsigned int)idx);

          PdfDictionary *toAcroformFieldDict = &(toAcroformFieldObj->GetDictionary());
          PdfDictionary *toLevelRootFieldDict = &(toLevelRootFieldObj->GetDictionary());

          PdfObject *toAcroformFieldTObj = toAcroformFieldDict->FindKey(PdfName("T"));
          PdfObject *toLevelRootFieldTObj = toLevelRootFieldDict->FindKey(PdfName("T"));

          const PdfString *toAcroformFieldT = &(toAcroformFieldTObj->GetString());
          const PdfString *toLevelRootFieldT = &(toLevelRootFieldTObj->GetString());

          const std::string toAcroformFieldTString = toAcroformFieldT->GetString();
          const std::string toLevelRootFieldTString = toLevelRootFieldT->GetString();

          if (toAcroformFieldTString == toLevelRootFieldTString)
          {
            toExistsLevelRootFieldObj = toAcroformFieldObj;
            break;
          }
        }

        if (toExistsLevelRootFieldObj == nullptr)
        {
          toAcroformFields->AddIndirect(*toLevelRootFieldObj);
          delete toLevelFeilds;
          continue;
        }

        PdfObject *toExistsLevelParentFieldObj = toExistsLevelRootFieldObj;
        for (size_t idx = 1; idx < toLevelFeilds->size(); idx++)
        {
          PdfObject *toLevelFieldObj = toLevelFeilds->at(idx);
          PdfDictionary *toLevelFieldDict = &(toLevelFieldObj->GetDictionary());

          PdfObject *toLevelFieldTObj = toLevelFieldDict->FindKey(PdfName("T"));
          const PdfString *toLevelFieldT = &(toLevelFieldTObj->GetString());
          const std::string toLevelFieldTString = toLevelFieldT->GetString();

          PdfObject *toExistsLevelFieldObj = nullptr;

          PdfDictionary *toExistsLevelParentDict = &(toExistsLevelParentFieldObj->GetDictionary());

          PdfObject *toExistsLevelParentFieldKidsObj = toExistsLevelParentDict->FindKey(PdfName("Kids"));
          PdfArray *toExistsLevelParentFieldKids = &(toExistsLevelParentFieldKidsObj->GetArray());

          bool levelExists = false;
          for (size_t iidx = 0; iidx < toExistsLevelParentFieldKids->size(); iidx++)
          {
            PdfObject *toExistsLevelParentFieldKidObj = toExistsLevelParentFieldKids->FindAt((unsigned int)iidx);
            PdfDictionary *toExistsLevelParentFieldKidDict = &(toExistsLevelParentFieldKidObj->GetDictionary());

            PdfObject *toExistsLevelParentFieldKidTObj = toExistsLevelParentFieldKidDict->FindKey(PdfName("T"));
            const PdfString *toExistsLevelParentFieldKidT = &(toExistsLevelParentFieldKidTObj->GetString());
            const std::string toExistsLevelParentFieldKidTString = toExistsLevelParentFieldKidT->GetString();

            if (toLevelFieldTString == toExistsLevelParentFieldKidTString)
            {
              toExistsLevelParentFieldObj = toExistsLevelParentFieldKidObj;
              levelExists = true;
            }
          }

          if (levelExists)
          {
            continue;
          }

          if (toLevelFieldDict->HasKey(PdfName("Parent")))
          {
            toLevelFieldDict->RemoveKey(PdfName("Parent"));
          }

          toLevelFieldDict->AddKeyIndirect(PdfName("Parent"), *toExistsLevelParentFieldObj);

          toExistsLevelParentFieldKids->AddIndirect(*toLevelFieldObj);
          break;
        }

        delete toLevelFeilds;
      }
    }
  }

  void PiPiPager::CopyPageAcroformFont(PdfMemDocument *to, PdfMemDocument *from, unsigned int base, unsigned int start, unsigned int end)
  {
    spdlog::trace("CopyPageAcroformFont");

    std::map<std::string, std::string> *fontMap = new std::map<std::string, std::string>();

    PdfAcroForm *fromAcroform = &(from->GetOrCreateAcroForm());
    PdfDictionary *fromAcroformDict = &(fromAcroform->GetDictionary());

    PdfObject *fromAcroformDrObj = fromAcroformDict->FindKey(PdfName("DR"));
    PdfDictionary *fromAcroformDr = &(fromAcroformDrObj->GetDictionary());

    PdfObject *fromAcroformDrFontObj = fromAcroformDr->FindKey(PdfName("Font"));
    PdfDictionary *fromAcroformDrFont = &(fromAcroformDrFontObj->GetDictionary());

    PdfPageCollection *fromPages = &(from->GetPages());
    for (unsigned int i = 0; i < fromPages->GetCount(); i++)
    {
      PdfPage *fromPage = &(fromPages->GetPageAt(i));
      PdfAnnotationCollection *fromAnnots = &(fromPage->GetAnnotations());
      for (unsigned int j = 0; j < fromAnnots->GetCount(); j++)
      {
        PdfAnnotation *toAnnot = &(fromAnnots->GetAnnotAt(j));

        std::string fromFontName = PiPiAnnotationUtil::ExtractAnnotationFontName(toAnnot);
        if (fromFontName == "")
        {
          continue;
        }

        auto fontMapFindIterator = fontMap->find(fromFontName);
        if (fontMapFindIterator != fontMap->end())
        {
          continue;
        }

        PdfObject *fromFontObj = fromAcroformDrFont->FindKey(PdfName(fromFontName));
        if (fromFontObj == nullptr)
        {
          continue;
        }

        PdfDictionary *fromFontDict = &(fromFontObj->GetDictionary());

        PdfObject *fromFontBaseFontObj = fromFontDict->FindKey(PdfName("BaseFont"));
        if (fromFontBaseFontObj == nullptr)
        {
          continue;
        }

        const PdfName *fromFontBaseFont = &(fromFontBaseFontObj->GetName());

        std::string fromBaseFontName = fromFontBaseFont->GetString();

        fontMap->insert(std::pair<std::string, std::string>(fromFontName, fromBaseFontName));
      }
    }

    PdfIndirectObjectList *toIndirectObjectList = &(to->GetObjects());

    PdfAcroForm *toAcroform = &(to->GetOrCreateAcroForm());
    PdfDictionary *toAcroformDict = &(toAcroform->GetDictionary());

    PdfObject *toAcroformDrObj = toAcroformDict->FindKey(PdfName("DR"));
    PdfDictionary *toAcroformDr = &(toAcroformDrObj->GetDictionary());

    PdfObject *toAcroformDrFontObj = toAcroformDr->FindKey(PdfName("Font"));
    PdfDictionary *toAcroformDrFont = &(toAcroformDrFontObj->GetDictionary());

    for (auto iterator = fontMap->begin(); iterator != fontMap->end(); iterator.operator++())
    {
      std::string fontName = iterator->first;
      std::string baseFontName = iterator->second;

      PdfObject *fontObj = nullptr;
      for (auto toIndirectIterator = toIndirectObjectList->begin(); toIndirectIterator != toIndirectObjectList->end(); toIndirectIterator.operator++())
      {
        PdfObject *toIndirectObject = *toIndirectIterator;

        if (!toIndirectObject->IsDictionary())
        {
          continue;
        }

        PdfDictionary *toIndirectDict = &(toIndirectObject->GetDictionary());

        if (!toIndirectDict->HasKey(PdfName("Type")))
        {
          continue;
        }

        PdfObject *toIndirectTypeObj = toIndirectDict->FindKey(PdfName("Type"));

        if (!toIndirectTypeObj->IsName())
        {
          continue;
        }

        const PdfName *toIndirectType = &(toIndirectTypeObj->GetName());

        if (*toIndirectType != PdfName("Font"))
        {
          continue;
        }

        PdfObject *toIndirectBaseFontEncoding = toIndirectDict->FindKey(PdfName("Encoding"));
        if (toIndirectBaseFontEncoding == nullptr)
        {
          continue;
        }

        PdfObject *toIndirectBaseFontObj = toIndirectDict->FindKey(PdfName("BaseFont"));
        if (toIndirectBaseFontObj == nullptr)
        {
          continue;
        }

        const PdfName *toIndirectBaseFont = &(toIndirectBaseFontObj->GetName());

        const std::string toBaseFontName = toIndirectBaseFont->GetString();

        if (toBaseFontName == baseFontName)
        {
          fontObj = toIndirectObject;
        }
      }

      if (fontObj == nullptr)
      {
        continue;
      }

      toAcroformDrFont->AddKeyIndirect(PdfName(fontName), *fontObj);
    }

    delete fontMap;
  }
}
