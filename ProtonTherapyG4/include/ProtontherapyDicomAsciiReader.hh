#ifndef ProtontherapyDicomAsciiReader_hh
#define ProtontherapyDicomAsciiReader_hh

class ProtontherapyDicomAsciiReader
{
	public:
		ProtontherapyDicomAsciiReader();
		~ProtontherapyDicomAsciiReader();
		const vector<G4double>& GetMasterHUData() const;

		vector<G4double> fMasterHUData; 
		G4int fnumSlice;
	private:
		void ReadAsciiFile(G4String);
		void ReadToMemory();
		void InitializeMasterData(G4int,G4int,G4int);
		G4int CompressIndices(G4int,G4int,G4int);

		G4String baseDirectory;
};
#endif