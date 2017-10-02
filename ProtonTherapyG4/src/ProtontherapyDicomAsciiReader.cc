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
		string line;
    G4String fname_asciiFile;
		G4ThreeVector refPosition;

		getline(fname_specsFile,line); //Get rid of header

		while(!fname_specsFile.eof()) {
      // Read the specs file and save the file name and ref position as vector
      // The rest of the parameters are assumed to be equal for all file for
      // "well-behaved" file type as checked by python script, dicomHandler.py

			fname_specsFile >> fname_asciiFile; // Name of Ascii file to read
      fname_asciiFileVector.push_back(fname_asciiFile);
			fname_specsFile >> sliceThickness; // in mm
			fname_specsFile >> ncol;
			fname_specsFile >> nrow;
			fname_specsFile >> pixelSpacing_col;
			fname_specsFile >> pixelSpacing_row;
			fname_specsFile >> refPosition[0];
			fname_specsFile >> refPosition[1];
			fname_specsFile >> refPosition[2];
      refPositionVector.push_back(refPosition);
			fname_specsFile >> dirCosine_col[0];
			fname_specsFile >> dirCosine_col[1];
			fname_specsFile >> dirCosine_col[2];
			fname_specsFile >> dirCosine_row[0];
			fname_specsFile >> dirCosine_row[1];
			fname_specsFile >> dirCosine_row[2];

      fnumSlice++;
    }
  }
  // Initialize the size of the vector of MasterData
  InitializeMasterData(ncol,nrow,fnumSlice);

  for(G4int k=0;k<fname_asciiFileVector.size();k++) {
		fname_asciiFile = baseDirectory + "/" + fname_asciiFileVector[k];
		ReadAsciiFile(fname_asciiFile,fnumSlice-k-1);
    g4cout << "Finished Processing Dicom File Number " << fnumSlice << "..." << g4endl;
  }
}

void ProtontherapyDicomAsciiReader::ReadAsciiFile(G4String fname, G4int zSlice) {
  ifstream readFile(fname);
  G4int HUvalue, index;
  if (readFile.is_open()) {
    for(G4int k=0;k<nrow;k++) {
      for(G4int kk=0;kk<ncol;kk++) {
        readFile >> HUvalue;
        index = CompressIndices(k,kk,zSlice);
        fMasterHUData[index] = HUvalue;
      }
    }
  }
}

void ProtontherapyDicomAsciiReader::InitializeMasterData(G4int x, G4int y, G4int z) {
  fMasterHUData.resize(x*y*z);
}

vector<G4double> ProtontherapyDicomAsciiReader::GetMasterHUData() {
  return fMasterHUData;
}

vector<G4ThreeVector> ProtontherapyDicomAsciiReader::GetSliceRefPosition() {
  return refPositionVector;
}

G4ThreeVector ProtontherapyDicomAsciiReader::GetPixelSize() {
  G4ThreeVector pixelSizeVector;
  pixelSizeVector[0] = pixelSpacing_row;
  pixelSizeVector[1] = pixelSpacing_col;
  pixelSizeVector[2] = sliceThickness;
  return pixelSizeVector;
}

G4ThreeVector ProtontherapyDicomAsciiReader::GetNumberOfPixels() {
  G4ThreeVector pixelNumberVector;
  pixelNumberVector[0] = nrow;
  pixelNumberVector[1] = ncol;
  pixelNumberVector[2] = fnumSlice;
  return pixelNumberVector;
}

G4ThreeVector ProtontherapyDicomAsciiReader::GetDirectionCosine_row() {
  return dirCosine_row;
}

G4ThreeVector ProtontherapyDicomAsciiReader::GetDirectionCosine_col() {
  return dirCosine_col;
}

G4int ProtontherapyDicomAsciiReader::CompressIndices(G4int x, G4int y, G4int z){
  return x + nrow*y + ncol*nrow*z;
}
