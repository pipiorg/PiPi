#include "main.h"

using namespace std;

void readFile(string filePath, char **fileBytes, size_t *fileSize)
{
  vector<char> vector;

  ifstream file;
  file.open(filePath, ios::binary);

  while (!file.eof())
  {
    char character;
    file.read(&character, 1);
    vector.push_back(character);
  }

  file.close();

  *fileSize = vector.size();
  *fileBytes = new char[vector.size()];
  for (long i = 0; i < vector.size(); i++)
  {
    *(*fileBytes + i) = vector[i];
  }

  cout << "Read FileSize: " << *fileSize << endl;
}

void writeFile(string filePath, char *fileBytes, size_t fileSize)
{
  ofstream file;
  file.open(filePath, ios::binary);

  for (size_t i = 0; i < fileSize; i++)
  {
    file.write(&fileBytes[i], 1);
  }

  cout << "Write FileSize: " << fileSize << endl;
}

void vector2array(vector<char> *vector, char **fileBytes, size_t *fileSize)
{
  *fileSize = vector->size();
  *fileBytes = new char[*fileSize];
  for (size_t i = 0; i < *fileSize; i++)
  {
    *(*fileBytes + i) = vector->at(i);
  }
}

void testMultile(char *file1Bytes, size_t file1Size, char *file2Bytes, size_t file2Size, char *newFileBytes, size_t newFileSize, char *fontBytes, size_t fontSize)
{
  PiPi::PiPiMultiOperator *multiOp = new PiPi::PiPiMultiOperator();
  size_t file1Index = multiOp->Add(file1Bytes, file1Size);
  size_t file2Index = multiOp->Add(file2Bytes, file2Size);

  PiPi::PiPiPager *pager = multiOp->GetPager();

  std::vector<size_t> *indexs = new std::vector<size_t>();
  indexs->push_back(0);
  indexs->push_back(1);

  std::vector<char> *out;
  pager->Merge(indexs, &out);

  vector2array(out, &newFileBytes, &newFileSize);
  writeFile("", newFileBytes, newFileSize);

  delete out;

  delete multiOp;
}

void testSingle(char *file1Bytes, size_t file1Size, char *newFileBytes, size_t newFileSize, char *fontBytes, size_t fontSize)
{
  PiPi::PiPiOperator *op = new PiPi::PiPiOperator(file1Bytes, file1Size);

  PiPi::PiPiEditor *editor = op->GetEditor();
  PiPi::PiPiFiller *filler = op->GetFiller();
  PiPi::PiPiExtractor *extractor = op->GetExtractor();
  PiPi::PiPiFontManager *fontManager = op->GetFontManager();

  // Do PDF operation

  std::vector<char> *out;
  op->Finalize(&out);

  vector2array(out, &newFileBytes, &newFileSize);
  writeFile("", newFileBytes, newFileSize);

  delete out;

  delete op;
}

int main()
{
  char *file1Bytes = nullptr;
  size_t file1Size = 0;

  char *file2Bytes = nullptr;
  size_t file2Size = 0;

  char *newFileBytes = nullptr;
  size_t newFileSize = 0;

  char *imageBytes = nullptr;
  size_t imageSize = 0;

  char *fontBytes = nullptr;
  size_t fontSize = 0;

  // Replace with your pdf and image file path
  // readFile("", &file1Bytes, &file1Size);
  // readFile("", &file2Bytes, &file2Size);
  // readFile("", &fontBytes, &fontSize);
  // readFile("", &imageBytes, &imageSize);

  // Test for single pdf operation
  // testSingle(file1Bytes, file1Size, newFileBytes, newFileSize, fontBytes, fontSize);

  // Test for multi pdf operation
  // testMultile(file1Bytes, file1Size, file2Bytes, file2Size, newFileBytes, newFileSize, fontBytes, fontSize);

  delete[] file1Bytes;
  delete[] file2Bytes;
  delete[] newFileBytes;
  delete[] fontBytes;
  delete[] imageBytes;

  return 0;
}
