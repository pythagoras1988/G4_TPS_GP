#include "globals.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include <string>

using namespace std;

#ifndef ProtontherapyDicomAsciiReader_hh
#define ProtontherapyDicomAsciiReader_hh

class ProtontherapyDicomAsciiReader
{
	public:
		ProtontherapyDicomAsciiReader();
		~ProtontherapyDicomAsciiReader();
		vector<G4double> GetMasterHUData();
		vector<G4ThreeVector> GetSliceRefPosition();
		G4ThreeVector GetPixelSize();
		G4ThreeVector GetNumberOfPixels();
		G4ThreeVector GetDirectionCosine_row();
		G4ThreeVector GetDirectionCosine_col();

		G4int fnumSlice;
	private:
		void ReadAsciiFile(string, G4int);
		void ReadToMemory();
		void InitializeMasterData(G4int,G4int,G4int);
		G4int CompressIndices(G4int,G4int,G4int);

		string baseDirectory;
		G4int ncol, nrow;
		G4double sliceThickness, pixelSpacing_row, pixelSpacing_col;
		G4ThreeVector dirCosine_row, dirCosine_col;
		vector<G4double> fMasterHUData;
		vector<string> fname_asciiFileVector;
		vector<G4ThreeVector> refPositionVector;

};
#endif
