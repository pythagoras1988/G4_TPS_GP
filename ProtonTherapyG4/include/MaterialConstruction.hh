#include "G4Material.hh"

#ifndef MaterialConstruction_hh
#define MaterialConstruction_hh

class MaterialConstruction {
  public:
    MaterialConstruction();
    ~MaterialConstruction();
    vector<G4Material*> GetMaterialSets();
    vector<G4double> GetHUThresholdVector();
  private:
    void CreateElements();
    void CreateMaterials();
    void CreateElementalWeights();
    void ConfigureMaterials(G4material*, G4double,G4double,G4double,G4double,
                            G4double,G4double,G4double,G4double,
                            G4double,G4double,G4double,G4double);
    vector<G4Material*> fMaterialSets;
    vector<G4double> fHUThresholdVector;
    map<G4double,vector<G4double>> fElementalWeights; 
    G4int numBins;
    g4double binSize;
    G4Element* elH, elC, elN, elO, elNa, elMg, elP,
              elS, elCl, elAr, elK, elCa;
};

#endif
