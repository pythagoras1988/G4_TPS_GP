#include "MaterialConstruction.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4SystemOfUnits.hh"
#include <vector>

MaterialConstruction::MaterialConstruction() {
  CreateElements();
  CreateMaterials();
}

MaterialConstruction::~MaterialConstruction()
{
}

void MaterialConstruction::CreateElements() {

  // Create Elements
  G4double z, a, density;
  G4String name, symbol;

  elC = new G4Element( name = "Carbon",
                       symbol = "C",
                       z = 6.0, a = 12.011 * g/mole );
  elH = new G4Element( name = "Hydrogen",
                       symbol = "H",
                       z = 1.0, a = 1.008  * g/mole );
  elN = new G4Element( name = "Nitrogen",
                       symbol = "N",
                       z = 7.0, a = 14.007 * g/mole );
  elO = new G4Element( name = "Oxygen",
                       symbol = "O",
                       z = 8.0, a = 16.00  * g/mole );
  elNa = new G4Element( name = "Sodium",
                        symbol = "Na",
                        z= 11.0, a = 22.98977* g/mole );
  elS = new G4Element( name = "Sulfur",
                       symbol = "S",
                       z = 16.0,a = 32.065* g/mole );
  elCl = new G4Element( name = "Chlorine",
                        symbol = "P",
                        z = 17.0, a = 35.453* g/mole );
  elK = new G4Element( name = "Potassium",
                       symbol = "P",
                       z = 19.0, a = 39.0983* g/mole );
  elP = new G4Element( name = "Phosphorus",
                       symbol = "P",
                       z = 15.0, a = 30.973976* g/mole );

  elMg = new G4Element( name = "Magnesium",
                        symbol = "Mg",
                        z = 12.0, a = 24.3050* g/mole );
  elCa = new G4Element( name="Calcium",
                        symbol = "Ca",
                        z = 20.0, a = 40.078* g/mole );
}

void MaterialConstruction::CreateMaterials() {
  //Create Materials according to Schneider's method
  G4String materialName;
  G4double meanHu;
  G4double density;
  numBins = 24;
  fHUThresholdVector.reserve(numBins+1);
  fHUThresholdVector = {-1000, -950, -120, -83, -53,
                        -23, 7, 18, 80, 120,
                        200, 300, 400, 500, 600,
                        700, 800, 900, 1000, 1100,
                        1200, 1300, 1400, 1500, 1600};
  fMaterialSets.reserve(numBins);

  for(G4int k; k<numBins; k++) { 
    fMaterialSets.push_back(new G4Material(materialName,density,ncomponents=12));
    ConfigureMaterials(fMaterialSets[k],);
  }
}

void MaterialConstruction::ConfigureMaterials(G4Material* material,
                                              G4double fmH, G4double fmC, G4double fmN,
                                              G4double fmO, G4double fmNa, G4double fmMg,
                                              G4double fmP, G4double fmS, G4double fmCl,
                                              G4double fmAr, G4double fmK, G4double fmCa)
{
  material->AddElement(elH,fractionmass=fmH*perCent);
  material->AddElement(elC,fractionmass=fmC*perCent);
  material->AddElement(elN,fractionmass=fmN*perCent);
  material->AddElement(elO,fractionmass=fmO*perCent);
  material->AddElement(elNa,fractionmass=fmNa*perCent);
  material->AddElement(elMg,fractionmass=fmMg*perCent);
  material->AddElement(elP,fractionmass=fmP*perCent);
  material->AddElement(elCl,fractionmass=fmCl*perCent);
  material->AddElement(elAr,fractionmass=fmAr*perCent);
  material->AddElement(elK,fractionmass=fmK*perCent);
  material->AddElement(elCa,fractionmass=fmCa*perCent);
  material->AddElement(elS,fractionmass=fmS*perCent);
}

vector<G4Material*> MaterialConstruction::GetMaterialSets() {
  return fMaterialSets;
}

vector<G4double> MaterialConstruction::GetHUThresholdVector() {
  return fHUThresholdVector;
}
