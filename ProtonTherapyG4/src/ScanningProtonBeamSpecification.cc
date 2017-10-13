#include "ScanningProtonBeamSpecification.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include <fstream>
#include <vector>
#include <map>
#include <string>

using namespace std;

ScanningProtonBeamSpecification::ScanningProtonBeamSpecification()
: fDirectory("Spots List"),
  fConstantWeightValue(0.1),
  fFluenceConstant(10000),
  fTotalWeight(0),
  constantWeight(true), 
  constantEnergy(true)
{
  ReadEnergyListFile();
  ReadWeightDataToMemory();
}

ScanningProtonBeamSpecification::~ScanningProtonBeamSpecification()
{
}

void ScanningProtonBeamSpecification::ReadEnergyListFile() {
  string tempString;
  ifstream readFile(fDirectory+"/List.txt");
  if(!readFile) {G4cout<<fDirectory+"/List.txt"<<G4endl;}
  if(readFile.is_open()){
    readFile >> tempString;
    while(!readFile.eof()) {
      getline(readFile,tempString,',');
      G4cout<<tempString<<G4endl;
      if (!tempString.empty())
        {energyList.push_back(stod(tempString));}
      getline(readFile,tempString);
    }
  }
  readFile.close();
  if (constantEnergy) { 
    //mainly for debugging use
    for(vector<G4double>::iterator it=energyList.begin();it!=energyList.end(); ++it) { 
      *it = 250.0; // MeV
    }
  }
  numEnergyLayers = energyList.size();
}

void ScanningProtonBeamSpecification::ReadWeightDataToMemory() {
  G4int counter = 0;
  G4double tempDouble;
  string tempString;
  vector<G4double> tempVector;
  numberOfEvents = 0;

  for(int k=0; k<numEnergyLayers; k++) {
    string fname(fDirectory + "/T_"+to_string((int)(energyList[k]*10000))+".txt");
    G4cout<<fname<<G4endl;
    ifstream readFile(fname);

    if (readFile.is_open()) {
      readFile>>tempString;
      while(!readFile.eof()) {
        tempVector.push_back(energyList[k]);
        for (int kk=0; kk<3; kk++) {
          if (kk!=2) {getline(readFile,tempString,',');}
          else {getline(readFile,tempString);}
          if(!tempString.empty()) {
            tempDouble = stod(tempString);
            if (constantWeight&&kk==2) {tempDouble = fConstantWeightValue;}
            if (kk==2) {
              tempDouble *= fFluenceConstant;
              numberOfEvents += tempDouble;
            }
            tempVector.push_back(tempDouble);
            //G4cout<<tempDouble<<G4endl;
          }
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
  return static_cast<G4int> numberOfEvents;
}


map<G4int,vector<G4double>> ScanningProtonBeamSpecification::GetWeightDataMap() {
  return fWeightData;
}

vector<double> ScanningProtonBeamSpecification::GetEnergyList() {
  return energyList;
}
