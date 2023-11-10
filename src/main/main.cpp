#include "main.h"

using namespace std;

void readFile(string filePath, char** fileBytes, size_t* fileSize) {
	vector<char> vector;

	ifstream file;
	file.open(filePath, ios::binary);

	while (!file.eof()) {
		char character;
		file.read(&character, 1);
		vector.push_back(character);
	}

	file.close();

	*fileSize = vector.size();
	*fileBytes = new char[vector.size()];
	for (long i = 0; i < vector.size(); i++) {
		*(*fileBytes + i) = vector[i];
	}

	cout << "Read FileSize: " << *fileSize << endl;
}

void writeFile(string filePath, char* fileBytes, size_t fileSize) {
	ofstream file;
	file.open(filePath, ios::binary);

	for (size_t i = 0; i < fileSize; i++) {
		file.write(&fileBytes[i], 1);
	}

	cout << "Write FileSize: " << fileSize << endl;
}

int main()
{
	char* fileBytes;
	size_t fileSize;

	char* newFileBytes;
	size_t newFileSize;

	char* imageBytes;
	size_t imageSize;

	// Replace with your pdf and image file path
	readFile("D:\\Empty.pdf", &fileBytes, &fileSize);
	readFile("D:\\OT.jpg", &imageBytes, &imageSize);

	PiPi::PiPiOperator* op = new PiPi::PiPiOperator(fileBytes, fileSize);
	PiPi::PiPiFiller* filler = op->getFiller();
	PiPi::PiPiEditor* editor = op->getEditor();
	PiPi::PiPiPager* pager = op->getPager();

	// Do PDF modification
	editor->renameFormField("TextField", "TTTT");

	op->finalize(&newFileBytes, &newFileSize);
	delete op;

	writeFile("D:\\Empty_o.pdf", newFileBytes, newFileSize);
}

