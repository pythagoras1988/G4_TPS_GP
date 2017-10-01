#ifndef ProtontherapyDicomAsciiReader_hh
#define ProtontherapyDicomAsciiReader_hh

class ProtontherapyDicomAsciiReader
{
	public:
		ProtontherapyDicomAsciiReader();
		~ProtontherapyDicomAsciiReader();

		G4double* fMasterHUData; 
	private:
		void ReadAsciiFile(G4String);
		void ReadToMemory();
		void InitializeMasterData(G4int,G4int,G4int);

		G4String baseDirectory;
};
#endif