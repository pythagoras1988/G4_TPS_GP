//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// Hadrontherapy advanced example for Geant4
// See more at: https://twiki.cern.ch/twiki/bin/view/Geant4/AdvancedExamplesHadrontherapy

#ifndef ScanningProtonBeamLine_H
#define ScanningProtonBeamLine_H 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include <set>

class G4VPhysicalVolume;
class ScanningProtonBeamLineMessenger;
class DicomDetectorConstruction;
class ProtontherapyDicomDetectorConstruction;
class ScanningProtonBeamLine : public G4VUserDetectorConstruction
{
public:

    ScanningProtonBeamLine();
    ~ScanningProtonBeamLine();

    G4VPhysicalVolume* Construct();

    virtual void ConstructSDandField();

    void SetScorer(G4LogicalVolume*);

    std::set<G4LogicalVolume*> fScorers;

    void ProtontherapyRangeShifter();
    // This defines the "range shifter". Is is a slab
    // (usually of PMMA" acting as energy degrader
    // of primary beam

    void ProtontherapyVacuumPipe();
    // This defines the Vacuum Chamber.

    void ProtontherapyMagnetX();
    // This is for magnets for x direction

    void ProtontherapyBeamMonitoring();
    // Definition of three monitor chambers

    void ProtontherapySpotPositionMonitorDetector();

    void ProtontherapyNozzleBoundary();

    // The following methods allow to change parameters
    // of some beam line components

    void SetRangeShifterXPosition(G4double value);
    // This method allows to move the Range Shifter along
    // the X axis

    void SetRangeShifterXSize(G4double halfSize);
    // This method allows to change the size of the range shifter along
    // the X axis

    void SetRSMaterial(G4String);
    // This method allows to change the material
    // of the range shifter

    void SetMagneticField(G4ThreeVector);


private:
    static ScanningProtonBeamLine* instance;
    //passive proton line dimensions
    void SetDefaultDimensions();
    void ConstructScanningProtonBeamLine();

    // geometry component
    ScanningProtonBeamLineMessenger* scanningMessenger;
    G4VPhysicalVolume* physicalTreatmentRoom;
    G4LogicalVolume* logicTreatmentRoom;
    DicomDetectorConstruction* protontherapyDetectorConstruction;


    G4LogicalVolume* logicMagnetX;
    G4VPhysicalVolume* physiMagnetX;

    G4Material* kapton;

    G4double vacuumZoneXSize;
    G4double vacuumZoneYSize;
    G4double vacuumZoneZSize;
    G4double vacuumZoneXPosition;

    G4double vacuumPipeXSize;
    G4double vacuumPipeYSize;
    G4double vacuumPipeZSize;
    G4double vacuumPipeXPosition;

    G4double kaptonWindowXSize;
    G4double kaptonWindowYSize;
    G4double kaptonWindowZSize;
    G4double kaptonWindowXPosition;

    G4double rangeShifterXSize;
    G4double rangeShifterYSize;
    G4double rangeShifterZSize;
    G4double rangeShifterXPosition;
    G4double rangeShifterYPosition;
    G4double rangeShifterZPosition;

    G4double magnetX_x;
    G4double magnetX_y;
    G4double magnetX_z;

    G4VPhysicalVolume* physiVacuumPipe1;
    G4VPhysicalVolume* physiVacuumZone1;
    G4VPhysicalVolume* physiVacuumPipe2;
    G4VPhysicalVolume* physiVacuumZone2;
    G4VPhysicalVolume* physiKaptonWindow;

    G4VPhysicalVolume* physiMagnet1;

    G4Box* solidRangeShifterBox;
    G4LogicalVolume* logicRangeShifterBox;
    G4VPhysicalVolume* physiRangeShifterBox;

    // Spot Position Monitor Detector
    // Mother volume
    G4VPhysicalVolume* physiSpotPositionMonitorMotherVolume;
    G4LogicalVolume* logicSpotPositionMonitorMotherVolume;
    G4Box* solidSpotPositionMonitorMotherVolume;

    G4double defaultSpotPositionMonitorYSize;
    G4double defaultSpotPositionMonitorZSize;

    // First Kapton layer
    G4double SpotPositionMonitorFirstKaptonLayerXSize;
    G4double SpotPositionMonitorFirstKaptonLayerYSize;
    G4double SpotPositionMonitorFirstKaptonLayerZSize;
    G4double SpotPositionMonitorFirstKaptonLayerXPosition;
    G4double SpotPositionMonitorFirstKaptonLayerYPosition;
    G4double SpotPositionMonitorFirstKaptonLayerZPosition;
    G4Box* solidSpotPositionMonitorFirstKaptonLayer;
    G4LogicalVolume* logicSpotPositionMonitorFirstKaptonLayer;
    G4VPhysicalVolume* physiSpotPositionMonitorFirstKaptonLayer;

    // First Aluminum layer
    G4double SpotPositionMonitorFirstAluminumLayerXSize;
    G4double SpotPositionMonitorFirstAluminumLayerYSize;
    G4double SpotPositionMonitorFirstAluminumLayerZSize;
    G4double SpotPositionMonitorFirstAluminumLayerXPosition;
    G4double SpotPositionMonitorFirstAluminumLayerYPosition;
    G4double SpotPositionMonitorFirstAluminumLayerZPosition;
    G4Box* solidSpotPositionMonitorFirstAluminumLayer;
    G4LogicalVolume* logicSpotPositionMonitorFirstAluminumLayer;
    G4VPhysicalVolume* physiSpotPositionMonitorFirstAluminumLayer;

    //First Copper Layer
    G4double SpotPositionMonitorFirstCopperLayerXSize;
    G4double SpotPositionMonitorFirstCopperLayerYSize;
    G4double SpotPositionMonitorFirstCopperLayerZSize;
    G4double SpotPositionMonitorFirstCopperLayerXPosition;
    G4double SpotPositionMonitorFirstCopperLayerYPosition;
    G4double SpotPositionMonitorFirstCopperLayerZPosition;
    G4Box* solidSpotPositionMonitorFirstCopperLayer;
    G4LogicalVolume* logicSpotPositionMonitorFirstCopperLayer;
    G4VPhysicalVolume* physiSpotPositionMonitorFirstCopperLayer;

    // First Argon Gap
    G4double SpotPositionMonitorFirstArgonLayerXSize;
    G4double SpotPositionMonitorFirstArgonLayerYSize;
    G4double SpotPositionMonitorFirstArgonLayerZSize;
    G4double SpotPositionMonitorFirstArgonLayerXPosition;
    G4double SpotPositionMonitorFirstArgonLayerYPosition;
    G4double SpotPositionMonitorFirstArgonLayerZPosition;
    G4Box* solidSpotPositionMonitorFirstArgonLayer;
    G4LogicalVolume* logicSpotPositionMonitorFirstArgonLayer;
    G4VPhysicalVolume* physiSpotPositionMonitorFirstArgonLayer;

    // Second Copper Layer
    G4double SpotPositionMonitorSecondCopperLayerXSize;
    G4double SpotPositionMonitorSecondCopperLayerYSize;
    G4double SpotPositionMonitorSecondCopperLayerZSize;
    G4double SpotPositionMonitorSecondCopperLayerXPosition;
    G4double SpotPositionMonitorSecondCopperLayerYPosition;
    G4double SpotPositionMonitorSecondCopperLayerZPosition;
    G4Box* solidSpotPositionMonitorSecondCopperLayer;
    G4LogicalVolume* logicSpotPositionMonitorSecondCopperLayer;
    G4VPhysicalVolume* physiSpotPositionMonitorSecondCopperLayer;

    // Second Aluminum layer
    G4double SpotPositionMonitorSecondAluminumLayerXSize;
    G4double SpotPositionMonitorSecondAluminumLayerYSize;
    G4double SpotPositionMonitorSecondAluminumLayerZSize;
    G4double SpotPositionMonitorSecondAluminumLayerXPosition;
    G4double SpotPositionMonitorSecondAluminumLayerYPosition;
    G4double SpotPositionMonitorSecondAluminumLayerZPosition;
    G4Box* solidSpotPositionMonitorSecondAluminumLayer;
    G4LogicalVolume* logicSpotPositionMonitorSecondAluminumLayer;
    G4VPhysicalVolume* physiSpotPositionMonitorSecondAluminumLayer;

    // Second Kapton layer
    G4double SpotPositionMonitorSecondKaptonLayerXSize;
    G4double SpotPositionMonitorSecondKaptonLayerYSize;
    G4double SpotPositionMonitorSecondKaptonLayerZSize;
    G4double SpotPositionMonitorSecondKaptonLayerXPosition;
    G4double SpotPositionMonitorSecondKaptonLayerYPosition;
    G4double SpotPositionMonitorSecondKaptonLayerZPosition;
    G4Box* solidSpotPositionMonitorSecondKaptonLayer;
    G4LogicalVolume* logicSpotPositionMonitorSecondKaptonLayer;
    G4VPhysicalVolume* physiSpotPositionMonitorSecondKaptonLayer;

    G4VPhysicalVolume* mother;

    G4VPhysicalVolume* physiFirstMonitorLayer1;
    G4VPhysicalVolume* physiFirstMonitorLayer2;
    G4VPhysicalVolume* physiFirstMonitorLayer3;
    G4VPhysicalVolume* physiFirstMonitorLayer4;
    G4VPhysicalVolume* physiFirstMonitorLayer5;
    G4VPhysicalVolume* physiSecondMonitorLayer1;
    G4VPhysicalVolume* physiSecondMonitorLayer2;
    G4VPhysicalVolume* physiSecondMonitorLayer3;
    G4VPhysicalVolume* physiSecondMonitorLayer4;
    G4VPhysicalVolume* physiSecondMonitorLayer5;

    G4VisAttributes* blue;
    G4VisAttributes* gray;
    G4VisAttributes* white;
    G4VisAttributes* red;
    G4VisAttributes* yellow;
    G4VisAttributes* green;
    G4VisAttributes* darkGreen;
    G4VisAttributes* darkOrange3;
    G4VisAttributes* skyBlue;
    G4VisAttributes* magenta;

    G4Material* rangeShifterMaterial;
    G4Material* vacuumZoneMaterial;
    G4Material* vacuumPipeMaterial;
    G4Material* kaptonWindowMaterial;
    G4Material* magnetMaterial;
    G4Material* layer1MonitorChamberMaterial;
    G4Material* layer2MonitorChamberMaterial;
    G4Material* layer3MonitorChamberMaterial;
    G4Material* layer4MonitorChamberMaterial;
    G4Material* layer5MonitorChamberMaterial;
    G4Material* SpotPositionMonitorMotherVolumeMaterial;
    G4Material* SpotPositionMonitorFirstKaptonLayerMaterial;
    G4Material* SpotPositionMonitorFirstAluminumLayerMaterial;
    G4Material* SpotPositionMonitorFirstArgonLayerMaterial;
    G4Material* SpotPositionMonitorFirstCopperLayerMaterial;
    G4Material* SpotPositionMonitorSecondCopperLayerMaterial;
    G4Material* SpotPositionMonitorSecondAluminumLayerMaterial;
    G4Material* SpotPositionMonitorSecondKaptonLayerMaterial;


};
#endif
