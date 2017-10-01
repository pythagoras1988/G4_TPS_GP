#include "ProtontherapyDicomAsciiReader.hh"
#include <vector> 
#include <fstream>
#include <string>

using namespace std;

ProtontherapyDicomAsciiReader::ProtontherapyDicomAsciiReader() 
: fnumSlice(0), 
  baseDirectory("LungRT01_ascii")
{
	ReadToMemory();
}

ProtontherapyDicomAsciiReader::~ProtontherapyDicomAsciiReader() 
{
}

void ProtontherapyDicomAsciiReader::ReadToMemory(){ 

	G4String fname_specsFile(baseDirectory+"_specsFile.txt");
	ifstream readFile(fname_specsFile);
	if(readFile.is_open()) { 
		G4String fname_asciiFile;
		string line; 
		G4int ncol, nrow; 
		G4double sliceThickness, pixelSpacing_row, pixelSpacing_col; 
		G4ThreeVector dirCosine_row, dirCosine_col, refPosition; 

		getline(fname_specsFile,line); //Get rid of header

		while(!fname_specsFile.eof()) {
			fname_specsFile >> fname_asciiFile; // Name of Ascii file to read
			fname_specsFile >> sliceThickness; // in mm
			fname_specsFile >> ncol; 
			fname_specsFile >> nrow; 
			fname_specsFile >> pixelSpacing_col; 
			fname_specsFile >> pixelSpacing_row; 
			fname_specsFile >> refPosition[0]; 
			fname_specsFile >> refPosition[1];
			fname_specsFile >> refPosition[2]; 
			fname_specsFile >> dirCosine_col[0];
			fname_specsFile >> dirCosine_col[1];
			fname_specsFile >> dirCosine_col[2];
			fname_specsFile >> dirCosine_row[0];
			fname_specsFile >> dirCosine_row[1];
			fname_specsFile >> dirCosine_row[2];

			if(fnumSlice==0) { 
				InitializeMasterData();
			}

			fname_asciiFile = baseDirectory + "/" + fname_asciiFile;
			ReadAsciiFile(fname_asciiFile);

			fnumSlice++; 
		}
	}

}

void ProtontherapyDicomAsciiReader::ReadAsciiFile(G4String fname)
{}

void ProtontherapyDicomAsciiReader::InitializeMasterData(G4int x, G4int y, G4int z)
{}