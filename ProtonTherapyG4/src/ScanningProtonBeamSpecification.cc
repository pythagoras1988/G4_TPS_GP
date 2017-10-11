#include "ScanningProtonBeamSpecification.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include <fstream>
#include <vector>
#include <map>
#include <string>

ScanningProtonBeamSpecification::ScanningProtonBeamSpecification()
: fDirectory("Spots List"),
  fConstantWeightValue(0.1),
  fFluenceConstant(10000),
  fTotalWeight(0),
  constantWeight(false)
{
  ReadEnergyListFile();
  ReadWeightDataToMemory();
}

ScanningProtonBeamSpecification::~ScanningProtonBeamSpecification()
{
}

void ScanningProtonBeamSpecification::ReadEnergyListFile() {
  string tempString;
  ifstream readFile(fDirectory+"/list.txt");
  if(readFile.is_open()){
    readFile >> tempString;
    while(!readFile.eof()) {
      getline(readFile,tempString,",");
      energyList.push_back((float) atof(tempString));
      getline(readFile,tempString);
    }
  }
  readFile.close()
  numEnergyLayers = energyList.size();
}

void ScanningProtonBeamSpecification::ReadWeightDataToMemory() {
  G4int counter = 0;
  G4double tempDouble;
  vector<G4double> tempVector;

  for(int k; k<numEnergyLayers; k++) {
    string fname("T_"+to_str(energyList[k]*10000)+".txt");
    ifstream readFile(fname); 

    if (readFile.is_open()) { 
      while(!readFile.eof()) { 
        tempVector.push_back(energyList[k]);
        for (int kk=0; kk<3; kk++) {
          readFile >> tempDouble; 
          if (constantWeight&&kk==2) {tempDouble = fConstantWeightValue;}
          if (kk==2) {tempDouble *= fFluenceConstant;}
          tempVector.push_back(tempDouble);
        }
        fWeightData[counter] = tempVector;
        tempVector.clear();
        counter++;
      }
    }
    readFile.close();
    G4cout<<"Finished Reading Weight data file "<<k<<G4endl;
  }

  if (fWeightData.size()==0) {
    G4Exception("ScanningProtonBeamSpecification","000",RunMustBeAborted,"No weight data from file!");
  }
}

G4int ScanningProtonBeamSpecification::GetNumberOfEvents() {
  return fWeightData.size();
}


map<G4int,vector<G4double>> GetWeightDataMap() {
  return fWeightData;
}

vector<double> ScanningProtonBeamSpecification::GetEnergyList() {
  return energyList;
}
