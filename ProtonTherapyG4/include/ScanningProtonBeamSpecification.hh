#include <vector>

#ifndef ScanningProtonBeamSpecification_hh
#define ScanningProtonBeamSpecification_hh 1

class ScanningProtonBeamSpecification {
  public:
    ScanningProtonBeamSpecification();
    ~ScanningProtonBeamSpecification();
  private:
    string fDirectory;
    int numEnergyLayers;
    vector<double> energyList;
    bool constantWeight;

    void CalibrateEnergy();
    void ReadEnergyListFile();
    void ReadWeightData();
};
#endif
