#include "MaterialConstruction.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4SystemOfUnits.hh"
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <math.h>

using namespace std;

MaterialConstruction::MaterialConstruction() {
  CreatePhantomElements();
  CreateElementalWeights();
  CreateMaterials();
}

MaterialConstruction::~MaterialConstruction()
{
  for (int k=0; k<(G4int) fMaterialSets.size();k++) {
    delete fMaterialSets[k];
  }
}

void MaterialConstruction::CreatePhantomElements() {

  // Create Elements
  G4double z, a;
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
  elAr = new G4Element( name="Argon",
                        symbol = "Ar",
                        z = 18.0, a = 39.948* g/mole );
}

void MaterialConstruction::CreateElementalWeights() {
  ifstream readFile("elemental_weights_Schneider.txt");
  if(readFile.is_open()) {
    string line;
    G4double startHU, endHU, weight;
    getline(readFile,line);

    while(!readFile.eof()) {
      readFile >> startHU;
      readFile >> endHU;
      for(G4int k=0; k<12; k++) {
        // read all the weight
        readFile >> weight;
        fElementalWeights[startHU].push_back(weight);
      }
      //G4cout<<startHU<<G4endl;
    }
  }
}

void MaterialConstruction::CreateMaterials() {
  //Create Materials according to Schneider's method
  vector<G4double> elementalWeight;
  G4String materialName;
  G4double startHU, stepHU;
  G4double density;
  G4int ncomponents;
  int counts(0);
  numBins = 260;
  binSize = 10;
  fHUThresholdVector.reserve(numBins+1);
  /*
  fHUThresholdVector = {-1000, -950, -120, -83, -53,
                        -23, 7, 18, 80, 120,
                        200, 300, 400, 500, 600,
                        700, 800, 900, 1000, 1100,
                        1200, 1300, 1400, 1500, 1600};
  */

  fMaterialSets.reserve(numBins+1);

  startHU = -1000; // Correspond to air
  while(startHU<=1600) {
    materialName = "material_" + to_string((G4int) startHU);
    // ****************determine the elemental composition******************************
    for (map<G4double,vector<G4double>>::iterator it = fElementalWeights.begin(); it!=fElementalWeights.end(); ++it) {
      if (it->first <= startHU) {stepHU = it->first;}  // stepHU is the elemental composition
      else {break;}
    }
    // ****************determine the density using Scheider's method********************
    if (startHU>=100) {
      density = (1.017 + 0.592*0.001*startHU) * g/cm3;
    }
    else if (startHU<-98) {
      density = (1.030*pow(10,-3)*startHU + 1.031) * g/cm3;
    }
    else if (startHU>14 && startHU<23) {
      density = 1.03*g/cm3;
    }
    else if (startHU>=-98 && startHU<=14){
      //equation (21) in Schneider's paper
      density = (1.018 + 0.893*0.001*startHU) * g/cm3;
    }
    else if (startHU>=23 && startHU<=100){
      //equation (23) in Schneider's paper
      density = (1.003 + 1.169*0.001*startHU) * g/cm3;
    }

    // ******************Push Material into Vector***************************************
    elementalWeight = fElementalWeights[stepHU];
    fMaterialSets.push_back(new G4Material(materialName,density,ncomponents=12));
    ConfigureMaterials(fMaterialSets[counts],elementalWeight[0],elementalWeight[1],elementalWeight[2],
                        elementalWeight[3], elementalWeight[4], elementalWeight[5],
                        elementalWeight[6], elementalWeight[7], elementalWeight[8],
                        elementalWeight[9], elementalWeight[10], elementalWeight[11]);
    fHUThresholdVector.push_back(startHU);

    startHU += binSize;
    G4cout << "Finished Material Construction for HU " << startHU <<G4endl;
    counts++;
  }
  //G4cout<<*(G4Material::GetMaterialTable())<<G4endl;
}

void MaterialConstruction::ConfigureMaterials(G4Material* material,
                                              G4double fmH, G4double fmC, G4double fmN,
                                              G4double fmO, G4double fmNa, G4double fmMg,
                                              G4double fmP, G4double fmS, G4double fmCl,
                                              G4double fmAr, G4double fmK, G4double fmCa)
{
  G4double fractionmass;

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
