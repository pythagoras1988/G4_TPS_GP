#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include <vector>
#include <map>

using namespace std;

#ifndef MaterialConstruction_H
#define MaterialConstruction_H 1

class MaterialConstruction {
  public:
    MaterialConstruction();
    ~MaterialConstruction();
    vector<G4Material*> GetMaterialSets();
    vector<G4double> GetHUThresholdVector();
  private:
    void CreatePhantomElements();
    void CreateMaterials();
    void CreateElementalWeights();
    void ConfigureMaterials(G4Material*, G4double,G4double,G4double,G4double,
                            G4double,G4double,G4double,G4double,
                            G4double,G4double,G4double,G4double);
    vector<G4Material*> fMaterialSets;
    vector<G4double> fHUThresholdVector;
    map<G4double,vector<G4double>> fElementalWeights;
    G4int numBins;
    G4double binSize;
    G4Element *elH, *elC, *elN, *elO, *elNa, *elMg, *elP,
              *elS, *elCl, *elAr, *elK, *elCa;
};

#endif
