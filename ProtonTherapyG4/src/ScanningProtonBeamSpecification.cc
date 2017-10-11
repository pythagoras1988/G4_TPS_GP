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
  if(!constantWeight) {ReadWeightDataToMemory();}
  else {ApplyConstantWeightScanning();}
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
  for(int k; k<numEnergyLayers; k++){
    string fname("T_"+to_str(energyList[k]*10000)+".txt");
    fWeightData[energyList[k]] = ReadEachWeightData(fDirectory+"/"+fname);
    G4cout<<"Finished Reading Weight data file "<<k<<G4endl;
  }
}

vector<vector<G4double>> ScanningProtonBeamSpecification::ReadEachWeightData(string fname) {
  vector<vector<G4double>> weightData;
  vector<G4double> tempVector;
  G4double tempDouble;
  totalWeight = 0;
  ifstream readFile(fname);

  if (readFile.is_open()) {
    while(!readFile.eof()) {
      for (int k=0;k<3;k++) {
        readFile>>tempDouble;
        if(constantWeight&&k==2) {
          tempDouble = fConstantWeightValue;
        }
        if(k==2) {
          tempVector.push_back(tempDouble*fFluenceConstant);
        }
        tempVector.push_back(tempDouble);
      }
      weightData.push_back(tempVector);
      tempVector.clear();
    }
  }
  readFile.close();
  if (weightData.size()==0) {
    G4Exception("ScanningProtonBeamSpecification","000",RunMustBeAborted,"No weight data from file!");
  }
  return weightData;
}

map<G4double,vector<vector<G4double>>> GetWeightData() {
  return fWeightData;
}

vector<double> ScanningProtonBeamSpecification::GetEnergyList() {
  return energyList;
}
