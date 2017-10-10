#include "ScanningProtonBeamSpecification.hh"
#include <fstream>
#include <vector>

ScanningProtonBeamSpecification::ScanningProtonBeamSpecification()
: fDirectory("Spots List"),
  constantWeight(false)
{
  ReadEnergyListFile();
}

ScanningProtonBeamSpecification::~ScanningProtonBeamSpecification()
{
}

ScanningProtonBeamSpecification::ReadEnergyListFile() {
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

ScanningProtonBeamSpecification::ReadWeightData() {
  
}
