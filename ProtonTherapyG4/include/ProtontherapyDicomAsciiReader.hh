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
		void ReadAsciiFile(G4String, G4int);
		void ReadToMemory();
		void InitializeMasterData(G4int,G4int,G4int);
		G4int CompressIndices(G4int,G4int,G4int);

		G4String baseDirectory;
		G4int ncol, nrow;
		G4double sliceThickness, pixelSpacing_row, pixelSpacing_col;
		G4ThreeVector dirCosine_row, dirCosine_col;
		vector<G4double> fMasterHUData;
		vector<G4String> fname_asciiFileVector;
		vector<G4ThreeVector> refPositionVector;

};
#endif
