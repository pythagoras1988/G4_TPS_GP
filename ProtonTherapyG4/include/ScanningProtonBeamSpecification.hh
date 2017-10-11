#include <vector>
#include <map>
#include <string>

#ifndef ScanningProtonBeamSpecification_hh
#define ScanningProtonBeamSpecification_hh 1

class ScanningProtonBeamSpecification {
  public:
    ScanningProtonBeamSpecification();
    ~ScanningProtonBeamSpecification();

    map<G4double,vector<vector<G4double>>> GetWeightData();
    vector<double> GetEnergyList();

  private:
    string fDirectory;
    int numEnergyLayers;
    vector<double> energyList;
    bool constantWeight;
    G4double fFluenceConstant;
    G4double fConstantWeightValue;
    G4double fTotalWeight;
    map<G4double,vector<vector<G4double>>> fWeightData;

    void CalibrateEnergy();
    void ReadEnergyListFile();
    void ReadWeightDataToMemory();
    vector<vector<G4double>> ReadEachWeightData(string);

};
#endif
