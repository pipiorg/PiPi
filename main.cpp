#include "main.h"
#include "podofo/podofo.h"

using namespace std;

int main()
{
	PoDoFo::PdfMemDocument * document = new PoDoFo::PdfMemDocument();
	
	string filename = "D:\\PODOFO.pdf";

	document->Save(filename);

	delete document;
}
