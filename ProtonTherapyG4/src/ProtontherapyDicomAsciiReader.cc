#include "ProtontherapyDicomAsciiReader.hh"
#include "globals.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include <fstream>
#include <string>
#include <float.h>

using namespace std;

ProtontherapyDicomAsciiReader::ProtontherapyDicomAsciiReader()
: fnumSlice(0),
  baseDirectory("LungRT01"),
  refPositionVector(0)
{
	ReadToMemory();
}

ProtontherapyDicomAsciiReader::~ProtontherapyDicomAsciiReader()
{
}

void ProtontherapyDicomAsciiReader::ReadToMemory(){

	string fname_specsFile(baseDirectory+"_specsFile.txt");
  string fname_asciiFile;
  G4double minZRefPosition(DBL_MAX);
  G4double maxZRefPosition(DBL_MIN);
  G4int indexZ(0);
	ifstream readFile(fname_specsFile);

	if(readFile.is_open()) {
		string line;
		G4ThreeVector refPosition;

		getline(readFile,line); //Get rid of header

		while(!readFile.eof()) {
      // Read the specs file and save the file name and ref position as vector
      // The rest of the parameters are assumed to be equal for all file for
      // "well-behaved" file type as checked by python script, dicomHandler.py

			readFile >> fname_asciiFile; // Name of Ascii file to read
      fname_asciiFileVector.push_back(fname_asciiFile);
			readFile >> sliceThickness; // in mm
			readFile >> ncol;
			readFile >> nrow;
			readFile >> pixelSpacing_col;
			readFile >> pixelSpacing_row;
			readFile >> refPosition[0];
			readFile >> refPosition[1];
			readFile >> refPosition[2];
      if (refPosition[2]<minZRefPosition) {minZRefPosition=refPosition[2];}
      if (refPosition[2]>maxZRefPosition) {maxZRefPosition=refPosition[2];}
      refPositionVector.push_back(refPosition);
	    readFile >> dirCosine_col[0];
			readFile >> dirCosine_col[1];
			readFile >> dirCosine_col[2];
			readFile >> dirCosine_row[0];
			readFile >> dirCosine_row[1];
			readFile >> dirCosine_row[2];

      fnumSlice++;
    }
  }
  // Initialize the size of the vector of MasterData
  InitializeMasterData(ncol,nrow,fnumSlice);

  for(G4int k=0;k<fname_asciiFileVector.size();k++) {
		fname_asciiFile = baseDirectory + "_ascii/" + fname_asciiFileVector[k];
    //G4cout<<fname_asciiFile<<G4endl;
    indexZ = static_cast<G4int> (refPositionVector[k][2]-minZRefPosition)/sliceThickness;
		ReadAsciiFile(fname_asciiFile,indexZ);
    G4cout << "Finished Processing Dicom File Number " << k << "..." << G4endl;
  }
}

void ProtontherapyDicomAsciiReader::ReadAsciiFile(string fname, G4int zSlice) {
  ifstream readFile(fname);
  if (!readFile){G4cout<<"No Ascii files..."<<G4endl;}

  G4int index;
  G4double HUvalue;
  if (readFile.is_open()) {
    G4cout<<"ok!!"<<G4endl;
    for(G4int k=0;k<nrow;k++) {
      for(G4int kk=0;kk<ncol;kk++) {
        readFile >> HUvalue;
        index = CompressIndices(k,kk,zSlice);
        fMasterHUData[index] = HUvalue;
        //if(kk==0) {G4cout<<HUvalue<<G4endl;}
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
  pixelNumberVector[2] = fnumSlice-1;
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
