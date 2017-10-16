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

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "G4NistElementBuilder.hh"
#include "ScanningProtonBeamLine.hh"
#include "ScanningProtonBeamLineMessenger.hh"
#include "DicomDetectorConstruction.hh"
#include "ProtontherapyDicomDetectorConstruction.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4HelixImplicitEuler.hh"

/////////////////////////////////////////////////////////////////////////////
ScanningProtonBeamLine::ScanningProtonBeamLine():
 physicalTreatmentRoom(0),protontherapyDetectorConstruction(0),physiVacuumPipe1(0),physiVacuumZone1(0), physiVacuumPipe2(0),physiVacuumZone2(0),physiKaptonWindow(0), physiMagnet1(0),
solidRangeShifterBox(0), logicRangeShifterBox(0), physiRangeShifterBox(0), physiSpotPositionMonitorMotherVolume(0),
physiFirstMonitorLayer1(0), physiFirstMonitorLayer2(0), physiFirstMonitorLayer3(0), physiFirstMonitorLayer4(0), physiFirstMonitorLayer5(0),
physiSecondMonitorLayer1(0), physiSecondMonitorLayer2(0), physiSecondMonitorLayer3(0), physiSecondMonitorLayer4(0), physiSecondMonitorLayer5(0),
fieldRotVector(0),fieldRotMatrix(0)
//physiHoleNozzleSupport(0),
{
    // Messenger to change parameters of the passiveProtonBeamLine geometry
    scanningMessenger = new ScanningProtonBeamLineMessenger(this);

}
/////////////////////////////////////////////////////////////////////////////
ScanningProtonBeamLine::~ScanningProtonBeamLine()
{
    delete scanningMessenger;
    //delete protontherapyDetectorConstruction;
}

/////////////////////////////////////////////////////////////////////////////
G4VPhysicalVolume* ScanningProtonBeamLine::Construct()
{
    // Sets default geometry and materials
    SetDefaultDimensions();

    // Construct the whole Passive Beam Line
    ConstructScanningProtonBeamLine();

    //***************************** PW ***************************************
    if (!protontherapyDetectorConstruction)
        //***************************** PW ***************************************
        // HadrontherapyDetectorConstruction builds ONLY the phantom and the detector with its associated ROGeometry
        protontherapyDetectorConstruction = new ProtontherapyDicomDetectorConstruction(logicTreatmentRoom,fieldRotVector);

    return physicalTreatmentRoom;
}

// In the following method the DEFAULTS used in the geometry of
// passive beam line are provided
// HERE THE USER CAN CHANGE THE GEOMETRY CHARACTERISTICS OF BEAM
// LINE ELEMENTS, ALTERNATIVELY HE/SHE CAN USE THE MACRO FILE (IF A
// MESSENGER IS PROVIDED)
//
// DEFAULT MATERIAL ARE ALSO PROVIDED
// and COLOURS ARE ALSO DEFINED
// ----------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::SetDefaultDimensions()
{
    // Set of coulors that can be used
    white = new G4VisAttributes( G4Colour());
    white -> SetVisibility(true);
    white -> SetForceSolid(true);

    blue = new G4VisAttributes(G4Colour(0. ,0. ,1.));
    blue -> SetVisibility(true);
    blue -> SetForceSolid(true);

    gray = new G4VisAttributes( G4Colour(0.5, 0.5, 0.5 ));
    gray-> SetVisibility(true);
    gray-> SetForceSolid(true);

    red = new G4VisAttributes(G4Colour(1. ,0. ,0.));
    red-> SetVisibility(true);
    red-> SetForceSolid(true);

    yellow = new G4VisAttributes(G4Colour(1., 1., 0. ));
    yellow-> SetVisibility(true);
    yellow-> SetForceSolid(true);

    green = new G4VisAttributes( G4Colour(25/255. , 255/255. ,  25/255. ));
    green -> SetVisibility(true);
    green -> SetForceSolid(true);

    darkGreen = new G4VisAttributes( G4Colour(0/255. , 100/255. ,  0/255. ));
    darkGreen -> SetVisibility(true);
    darkGreen -> SetForceSolid(true);

    darkOrange3 = new G4VisAttributes( G4Colour(205/255. , 102/255. ,  000/255. ));
    darkOrange3 -> SetVisibility(true);
    darkOrange3 -> SetForceSolid(true);

    skyBlue = new G4VisAttributes( G4Colour(135/255. , 206/255. ,  235/255. ));
    skyBlue -> SetVisibility(true);
    skyBlue -> SetForceSolid(true);

//    magenta = new G4VisAttributes( G4Colour(255. , 0. ,  255. ));
//    magenta -> SetVisibility(true);
//    magenta -> SetForceSolid(true);

    //***************************************************************************************************************
    // ALL COMPONENTS DIMENSIONS AND POSITIONS TO BE INPUT HERE!!!!
    //***************************************************************************************************************

    // Schematics of the Layer for Spot Position Detector monitor
    // *****************************
    // | P |   | A | P |   | A |   |  Polyimide film (Window)       = 12.5 um      position in treatment room = -538.0156 mm
    // | O | C | L | O | C | L |   |  Copper (Window coating)       = 0.2 um       position in treatment room = -538.0131 mm
    // | L | O | U | L | O | U | A |  Aluminum (window coating)     = 0.1 um       position in treatment room = -538.0129 mm
    // | Y | P | M | Y | P | M | R |  Polyimide film (Electrode)    = 12.5 um      position in treatment room = -538.0128 mm
    // | I | P | I | I | P | I | G |  Aluminum (Electrode)          = 0.2 um       position in treatment room = -538.0003 mm
    // | M | E | N | M | E | N | O |  Copper (Electrode)            = 0.1 um       position in treatment room = -538.0001 mm
    // | I | R | U | I | R | U | N |  Argon Gas (Ionization gas)    = 18 mm        position in treatment room = -538.0000 mm
    // | D |   | M | D |   | M |   |
    // | E |   |   | E |   |   |   |<-------This edge is just nice at -520 mm in the treatment room (matches the proton machine specs)
    // *****************************
    // Towards Isocenter along x-axis ------------>

    // Mother volume of Spot Position Monitor
    defaultSpotPositionMonitorYSize = 10.0*cm;
    defaultSpotPositionMonitorZSize = 10.0*cm;

    //Argon Gas Layer
    G4double defaultSpotPositionMonitorFirstArgonLayerXSize = 18 *mm;
    SpotPositionMonitorFirstArgonLayerXSize = defaultSpotPositionMonitorFirstArgonLayerXSize;

    G4double defaultSpotPositionMonitorFirstArgonLayerXPosition = -538 *mm - defaultSpotPositionMonitorFirstArgonLayerXSize/2; //************************************************ Positioning where? Ideally after the 2nd Aluminum Layer*****************
    SpotPositionMonitorFirstArgonLayerXPosition = defaultSpotPositionMonitorFirstArgonLayerXPosition;

    //Second Aluminum  Layer of Spot Position Monitor
    G4double defaultSpotPositionMonitorSecondAluminumLayerXSize = 0.1 *um / mm * 2 * 3;
    SpotPositionMonitorSecondAluminumLayerXSize = defaultSpotPositionMonitorSecondAluminumLayerXSize;

    G4double defaultSpotPositionMonitorSecondAluminumLayerXPosition = -538*mm - defaultSpotPositionMonitorFirstArgonLayerXSize
                                                                    - defaultSpotPositionMonitorSecondAluminumLayerXSize/2;
    SpotPositionMonitorSecondAluminumLayerXPosition = defaultSpotPositionMonitorSecondAluminumLayerXPosition;

    // Second Copper Layer of Spot Position Monitor
    G4double defaultSpotPositionMonitorSecondCopperLayerXSize = 0.2 *um / mm * 2 * 3;
    SpotPositionMonitorSecondCopperLayerXSize = defaultSpotPositionMonitorSecondCopperLayerXSize;

    G4double defaultSpotPositionMonitorSecondCopperLayerXPosition = -538*mm - defaultSpotPositionMonitorFirstArgonLayerXSize
                                                                    - defaultSpotPositionMonitorSecondAluminumLayerXSize
                                                                    - defaultSpotPositionMonitorSecondCopperLayerXSize/2;
    SpotPositionMonitorSecondCopperLayerXPosition = defaultSpotPositionMonitorSecondCopperLayerXPosition;

    // Second Kapton Layer of Spot Position Monitor
    G4double defaultSpotPositionMonitorSecondKaptonLayerXSize = 12.5 *um / mm * 3;
    SpotPositionMonitorSecondKaptonLayerXSize = defaultSpotPositionMonitorSecondKaptonLayerXSize;

    G4double defaultSpotPositionMonitorSecondKaptonLayerXPosition = -538*mm - defaultSpotPositionMonitorFirstArgonLayerXSize
                                                                    - defaultSpotPositionMonitorSecondAluminumLayerXSize
                                                                    - defaultSpotPositionMonitorSecondCopperLayerXSize
                                                                    - defaultSpotPositionMonitorSecondKaptonLayerXSize/2;
    SpotPositionMonitorSecondKaptonLayerXPosition = defaultSpotPositionMonitorSecondKaptonLayerXPosition;

    //First Aluminum  Layer of Spot Position Monitor
    G4double defaultSpotPositionMonitorFirstAluminumLayerXSize = 0.1 *um / mm * 2 * 2;
    SpotPositionMonitorFirstAluminumLayerXSize = defaultSpotPositionMonitorFirstAluminumLayerXSize;

    G4double defaultSpotPositionMonitorFirstAluminumLayerXPosition = -538*mm - defaultSpotPositionMonitorFirstArgonLayerXSize
                                                                    - defaultSpotPositionMonitorSecondAluminumLayerXSize
                                                                    - defaultSpotPositionMonitorSecondCopperLayerXSize
                                                                    - defaultSpotPositionMonitorSecondKaptonLayerXSize
                                                                    - defaultSpotPositionMonitorFirstAluminumLayerXSize/2;
    SpotPositionMonitorFirstAluminumLayerXPosition = defaultSpotPositionMonitorFirstAluminumLayerXPosition;

    //First Copper Layer of Spot Position Monitor
    G4double defaultSpotPositionMonitorFirstCopperLayerXSize = 0.2 *um / mm * 2 * 2;
    SpotPositionMonitorFirstCopperLayerXSize = defaultSpotPositionMonitorFirstCopperLayerXSize;

    G4double defaultSpotPositionMonitorFirstCopperLayerXPosition = -538*mm - defaultSpotPositionMonitorFirstArgonLayerXSize
                                                                    - defaultSpotPositionMonitorSecondAluminumLayerXSize
                                                                    - defaultSpotPositionMonitorSecondCopperLayerXSize
                                                                    - defaultSpotPositionMonitorSecondKaptonLayerXSize
                                                                    - defaultSpotPositionMonitorFirstCopperLayerXSize/2;
    SpotPositionMonitorFirstCopperLayerXPosition = defaultSpotPositionMonitorFirstCopperLayerXPosition;

    // First Kapton Layer of Spot Position Monitor
    G4double defaultSpotPositionMonitorFirstKaptonLayerXSize = 12.5 *um / mm * 2;
    SpotPositionMonitorFirstKaptonLayerXSize = defaultSpotPositionMonitorFirstKaptonLayerXSize;

    G4double defaultSpotPositionMonitorFirstKaptonLayerXPosition = -538*mm - defaultSpotPositionMonitorFirstArgonLayerXSize
                                                                    - defaultSpotPositionMonitorSecondAluminumLayerXSize
                                                                    - defaultSpotPositionMonitorSecondCopperLayerXSize
                                                                    - defaultSpotPositionMonitorSecondKaptonLayerXSize
                                                                    - defaultSpotPositionMonitorFirstCopperLayerXSize
                                                                    - defaultSpotPositionMonitorFirstKaptonLayerXSize/2;

    SpotPositionMonitorFirstKaptonLayerXPosition = defaultSpotPositionMonitorFirstKaptonLayerXPosition;



    // DEFAULT DEFINITION OF THE MATERIALS
    // All elements and compound definition follows the NIST database

    // ELEMENTS
    G4bool isotopes = false;
    G4Material* neodymiumNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_Nd", isotopes);
    G4Material* aluminumNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al", isotopes);
    //G4Material* tantalumNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ta", isotopes);
    G4Material* copperNistAsMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu", isotopes);
    //G4Element* zincNist = G4NistManager::Instance()->FindOrBuildElement("Zn");
    G4Material* copperNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu",isotopes);
    G4Material* argonNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ar",isotopes);
    G4Material* titaniumNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ti",isotopes);

    // COMPOUND
    G4Material* airNist =  G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR", isotopes);
    G4Material* kaptonNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON", isotopes);
    G4Material* galacticNist = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic", isotopes);
    //G4Material* PMMANist = G4NistManager::Instance()->FindOrBuildMaterial("G4_PLEXIGLASS", isotopes);

    G4double z, a, density;  // z is for atomic number, a for molar mass, density is density
    G4String name, symbol;
    G4int ncomponents, natoms; // ncomponents is the number of components inside the ABSresin
                               // natoms is the number of atoms inside the ABSresin

    a = 1.01*g/mole;
    G4Element* elH  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);

    a = 12.01*g/mole;
    G4Element* elC  = new G4Element(name="Carbon"  ,symbol="O" , z= 6., a);

    a = 14.00*g/mole;
    G4Element* elN  = new G4Element(name="Nitrogen" ,symbol="N", z= 7., a);

    density = 1.000*g/cm3;   // Need to change density value to the density of ABS resin
    G4Material* ABSresin = new G4Material(name="ABSresin",density,ncomponents=3);
    ABSresin->AddElement(elH, natoms=2); //Need to chang the number of Hydrogen atom according to the chemical formula of ABSresin
    ABSresin->AddElement(elC, natoms=1); //Need to chang the number of Carbon   atom according to the chemical formula of ABSresin
    ABSresin->AddElement(elN, natoms=3); //Need to chang the number of Nitrogen atom according to the chemical formula of ABSresin

    // MATERIAL ASSIGNMENT
    // Range shifter ********************************************Currently set to random composition need to change to ABS resin compound when given by hitachi
    rangeShifterMaterial = ABSresin;

    // Magnet Materials
    magnetMaterial = neodymiumNist;

    // Vacuum Zone
    vacuumZoneMaterial = galacticNist;

    // Vacuum Pipe
    vacuumPipeMaterial = titaniumNist;

    // Material of vacuum window
    kaptonWindowMaterial = titaniumNist;

    // Materials of the monitor chamber
    layer1MonitorChamberMaterial = kaptonNist;
    layer2MonitorChamberMaterial = copperNistAsMaterial;
    layer3MonitorChamberMaterial = kaptonNist;
    layer4MonitorChamberMaterial = copperNistAsMaterial;
    layer5MonitorChamberMaterial = aluminumNist;

    // Mother volume of the Spot Position detector
    SpotPositionMonitorMotherVolumeMaterial = airNist;
    SpotPositionMonitorFirstKaptonLayerMaterial = kaptonNist;
    SpotPositionMonitorFirstCopperLayerMaterial = copperNist;
    SpotPositionMonitorFirstAluminumLayerMaterial = aluminumNist;
    SpotPositionMonitorSecondKaptonLayerMaterial = kaptonNist;
    SpotPositionMonitorSecondCopperLayerMaterial = copperNist;
    SpotPositionMonitorSecondAluminumLayerMaterial = aluminumNist;
    SpotPositionMonitorFirstArgonLayerMaterial = argonNist;

}

/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::ConstructScanningProtonBeamLine()
{
    // -----------------------------
    // Treatment room - World volume
    //------------------------------
    // Treatment room sizes
    const G4double worldX = 400.0*cm ; //1200.0 *cm;
    const G4double worldY = 400.0 *cm; //1200*cm;
    const G4double worldZ = 500.0 *cm;
    G4bool isotopes = false;

    G4Material* airNist =  G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR", isotopes);
    G4Box* treatmentRoom = new G4Box("TreatmentRoom",worldX,worldY,worldZ);
    logicTreatmentRoom = new G4LogicalVolume(treatmentRoom,                        //Combine the materials and dimensions together
                                              airNist,
                                              "logicTreatmentRoom",
                                              0,0,0);
    physicalTreatmentRoom = new G4PVPlacement(0,                                                    //This is where I place the volume to be
                                              G4ThreeVector(),
                                              "physicalTreatmentRoom",
                                              logicTreatmentRoom,
                                              0,false,0);


    // The treatment room is invisible in the Visualisation
    logicTreatmentRoom -> SetVisAttributes (G4VisAttributes::GetInvisible());

    // Components of the Passive Proton Beam Line
    ProtontherapyMagnetX();
    ProtontherapyVacuumPipe();
    ProtontherapyBeamMonitoring();
    ProtontherapySpotPositionMonitorDetector();
    ProtontherapyRangeShifter();
    ProtontherapyNozzleBoundary();
}

void ScanningProtonBeamLine::ProtontherapyMagnetX() {

  magnetX_x = -1350.0 * mm;
  magnetX_y = 200.0 * mm;
  magnetX_z = magnetX_y;
  G4Box* solidMagnetX = new G4Box("magnetX",
                                       magnetX_y/2,
                                       magnetX_y/2,
                                       magnetX_y/2);

  logicMagnetX = new G4LogicalVolume(solidMagnetX,
                        vacuumZoneMaterial,
                        "magnetX");

  physiMagnetX = new G4PVPlacement(0, G4ThreeVector(magnetX_x, 0., 0.),
                        "magnetX", logicMagnetX,
                        physicalTreatmentRoom, false, 0);

  logicMagnetX -> SetVisAttributes(darkOrange3);
}


/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::ProtontherapyVacuumPipe()
{
    // ------------------//
    //    Vacuum Pipe    //
    //-------------------//

    // VACUUM WINDOW or KAPTON WINDOW: it permits the passage of the beam from vacuum to air
    G4double defaultKaptonWindowXSize = 0.1*mm;
    kaptonWindowXSize = defaultKaptonWindowXSize;

    G4double defaultKaptonWindowYSize = 15*cm;
    kaptonWindowYSize = defaultKaptonWindowYSize;

    G4double defaultKaptonWindowZSize = 15*cm;
    kaptonWindowZSize = defaultKaptonWindowZSize;

    G4double defaultKaptonWindowXPosition = (-770.0*mm + defaultKaptonWindowXSize/2) ; // The exact value does not matter if no field/material before window
    kaptonWindowXPosition = defaultKaptonWindowXPosition;


    // Vacuum Pipe and Vacuum Zone
    // The vacuum position depends on kapton window position and quadrupole
    G4double innerRadiusVacuumPipe = 75 *mm;
    G4double outerRadiusVacuumPipe = 85 *mm;
    G4double innerRadiusVacuumZone = 0 *mm;
    G4double outerRadiusVacuumZone = 75 *mm;

    G4double lengthVacuumPipe1 = (2613*mm + magnetX_x - magnetX_y/2);
    G4double VacuumPipeXPosition1 = (-2613*mm + (magnetX_x - magnetX_y/2))/2;
    G4double lengthVacuumZone1 = lengthVacuumPipe1;
    G4double VacuumZoneXPosition1 = VacuumPipeXPosition1;

    G4double lengthVacuumPipe2 = (-1*(magnetX_x + magnetX_y/2) + (-770*mm));
    G4double VacuumPipeXPosition2 = ((magnetX_x + magnetX_y/2) + (-770*mm))/2;
    G4double lengthVacuumZone2 = lengthVacuumPipe2;
    G4double VacuumZoneXPosition2 = VacuumPipeXPosition2;




    // Matrix definition for a 90 deg rotation. Also used for other volumes
    G4double phi = 90. *deg;
    G4RotationMatrix* rm = new G4RotationMatrix;
    rm->rotateY(phi);

    G4Tubs* solidVacuumPipe1 = new G4Tubs("VacuumPipe1",
                                        innerRadiusVacuumPipe,
                                        outerRadiusVacuumPipe,
                                        lengthVacuumPipe1/2,
                                        0,
                                        360*deg);

    G4LogicalVolume* logicVacuumPipe1 = new G4LogicalVolume(solidVacuumPipe1,
                                                          vacuumPipeMaterial,
                                                          "VacuumPipe1",
                                                          0, 0, 0);

    physiVacuumPipe1 = new G4PVPlacement(rm,G4ThreeVector(VacuumPipeXPosition1, 0., 0.),
                                       "VacuumPipe1",
                                       logicVacuumPipe1,
                                       physicalTreatmentRoom,
                                       false,
                                       0);

    logicVacuumPipe1 -> SetVisAttributes(darkOrange3);

    //Physical Definition for Vacuum Zone within the Pipe

    G4Tubs* solidVacuumZone1 = new G4Tubs("VacuumZone1",
                                        innerRadiusVacuumZone,
                                        outerRadiusVacuumZone,
                                        lengthVacuumZone1/2,
                                        0,
                                        360*deg);

    G4LogicalVolume* logicVacuumZone1 = new G4LogicalVolume(solidVacuumZone1,
                                                          vacuumZoneMaterial,
                                                          "VacuumZone1",
                                                          0, 0, 0);

    physiVacuumZone1 = new G4PVPlacement(rm,G4ThreeVector(VacuumZoneXPosition1, 0.,0.),
                                       "VacuumZone1",
                                       logicVacuumZone1,
                                       physicalTreatmentRoom,
                                       false,
                                       0);

    // The Vacuum Zone is invisible in the Visualisation
    logicVacuumZone1 -> SetVisAttributes (G4VisAttributes::GetInvisible());

    G4Tubs* solidVacuumPipe2 = new G4Tubs("VacuumPipe2",
                                        innerRadiusVacuumPipe,
                                        outerRadiusVacuumPipe,
                                        lengthVacuumPipe2/2,
                                        0,
                                        360*deg);

    G4LogicalVolume* logicVacuumPipe2 = new G4LogicalVolume(solidVacuumPipe2,
                                                          vacuumPipeMaterial,
                                                          "VacuumPipe2",
                                                          0, 0, 0);

    physiVacuumPipe2 = new G4PVPlacement(rm,G4ThreeVector(VacuumPipeXPosition2, 0., 0.),
                                       "VacuumPipe2",
                                       logicVacuumPipe2,
                                       physicalTreatmentRoom,
                                       false,
                                       0);

    logicVacuumPipe2 -> SetVisAttributes(darkOrange3);

    //Physical Definition for Vacuum Zone within the Pipe

    G4Tubs* solidVacuumZone2 = new G4Tubs("VacuumZone2",
                                        innerRadiusVacuumZone,
                                        outerRadiusVacuumZone,
                                        lengthVacuumZone2/2,
                                        0,
                                        360*deg);

    G4LogicalVolume* logicVacuumZone2 = new G4LogicalVolume(solidVacuumZone2,
                                                          vacuumZoneMaterial,
                                                          "VacuumZone2",
                                                          0, 0, 0);

    physiVacuumZone2 = new G4PVPlacement(rm,G4ThreeVector(VacuumZoneXPosition2, 0.,0.),
                                       "VacuumZone2",
                                       logicVacuumZone2,
                                       physicalTreatmentRoom,
                                       false,
                                       0);

    // The Vacuum Zone is invisible in the Visualisation
    logicVacuumZone2 -> SetVisAttributes (G4VisAttributes::GetInvisible());

    // -------------------//
    // THE VACUUM WINDOWS //
    //--------------------//
    //It permits the passage of the beam from vacuum to air
    G4Box* solidKaptonWindow = new G4Box("KaptonWindow",
                                         kaptonWindowXSize/2,
                                         kaptonWindowYSize/2,
                                         kaptonWindowZSize/2);

    G4LogicalVolume* logicKaptonWindow = new G4LogicalVolume(solidKaptonWindow,
                                                             kaptonWindowMaterial,
                                                             "KaptonWindow");

    physiKaptonWindow = new G4PVPlacement(0, G4ThreeVector(kaptonWindowXPosition, 0., 0.),
                                          "KaptonWindow", logicKaptonWindow,
                                          physicalTreatmentRoom, false, 0);

    logicKaptonWindow -> SetVisAttributes(darkOrange3);


}
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::ProtontherapyBeamMonitoring()
{
    // ----------------------------
    //   THE MAIN DOSE MONITOR CHAMBER
    // ----------------------------
    // The protontherapy machine consists of two monitor chambers,
    // one is the main dose monitor chamber and the other is the sub-
    // dose monitor chamber. Both of the chambers are made of the same
    // composition the only difference is the distances from the isocentre.
    // FIRST MONITOR CHAMBER = Main dose chamber
    // SECOND MONITOR CHAMBER = Sub dose chamber
    //
    // Schematics of the Layer for Main Dose monitor chamber
    // *********************
    // | P |   | P |   | A |   Polyimide film (Electrode) = 50 um               position in treatment room = -580.0648 mm
    // | O | C | O | C | L |   Copper (Electrode coating) = 2 um                position in treatment room = -580.0148 mm
    // | L | O | L | O | U |   Polyimide (window)         = 12.5 um             position in treatment room = -580.0128 mm
    // | Y | P | Y | P | M |   Copper (window coating)    = 0.2 um              position in treatment room = -580.0003 mm
    // | I | P | I | P | I |   Aluminum (Window coating)  = 0.1 um              position in treatment room = -580.0001 mm
    // | M | E | M | E | N |
    // | I | R | I | R | U |<-------This edge is just nice at -580 mm in the treatment room (matches the proton machine specs)
    // | D |   | D |   | M |
    // | E |   | E |   |   |
    // *********************
    // Towards Isocenter along x-axis ------------>
    //
    // Schematics of the Layer for Sub Dose monitor chamber
    // *********************
    // | P |   | P |   | A |   Polyimide film (Electrode) = 50 um               position in treatment room = -649.0648 mm
    // | O | C | O | C | L |   Copper (Electrode coating) = 2 um                position in treatment room = -649.0148 mm
    // | L | O | L | O | U |   Polyimide (window)         = 12.5 um             position in treatment room = -649.0128 mm
    // | Y | P | Y | P | M |   Copper (window coating)    = 0.2 um              position in treatment room = -649.0003 mm
    // | I | P | I | P | I |   Aluminum (Window coating)  = 0.1 um              position in treatment room = -649.0001 mm
    // | M | E | M | E | N |
    // | I | R | I | R | U |<-------This edge is just nice at -649 mm in the treatment room (matches the proton machine specs)
    // | D |   | D |   | M |
    // | E |   | E |   |   |
    // *********************
    // Towards Isocenter along x-axis ------------>


    const G4double monitor1XSize = 50 *um / mm * 3;
    const G4double monitor2XSize = 2 *um / mm * 2 * 3;
    const G4double monitor3XSize = 12.5 *um / mm * 2;
    const G4double monitor4XSize = 0.2 *um / mm * 2 * 2;
    const G4double monitor5XSize = 0.1 *um / mm * 2 * 2;
    const G4double monitorYSize = 10*cm / mm;
    const G4double monitorZSize = 10*cm / mm;

    const G4double subDoseStartPointX = -649.0*mm;
    const G4double mainDoseStartPointX = -580.0*mm;

    //Position for subdose monitor
    const G4double monitor10XPosition = subDoseStartPointX - monitor5XSize/2;
    const G4double monitor9XPosition = subDoseStartPointX - monitor5XSize - monitor4XSize/2;
    const G4double monitor8XPosition = subDoseStartPointX - monitor5XSize - monitor4XSize - monitor3XSize/2;
    const G4double monitor7XPosition = subDoseStartPointX - monitor5XSize - monitor4XSize - monitor3XSize - monitor2XSize/2;
    const G4double monitor6XPosition = subDoseStartPointX - monitor5XSize - monitor4XSize - monitor3XSize - monitor2XSize - monitor1XSize/2;

    //position for maindose monitor
    const G4double monitor5XPosition = mainDoseStartPointX - monitor5XSize/2;
    const G4double monitor4XPosition = mainDoseStartPointX - monitor5XSize - monitor4XSize/2;
    const G4double monitor3XPosition = mainDoseStartPointX - monitor5XSize - monitor4XSize - monitor3XSize/2;
    const G4double monitor2XPosition = mainDoseStartPointX - monitor5XSize - monitor4XSize - monitor3XSize - monitor2XSize/2;
    const G4double monitor1XPosition = mainDoseStartPointX - monitor5XSize - monitor4XSize - monitor3XSize - monitor2XSize - monitor1XSize/2;


    G4Box* solidFirstMonitorLayer1 = new G4Box("FirstMonitorLayer1",
                                               monitor1XSize/2,
                                               monitorYSize/2,
                                               monitorZSize/2);

    G4LogicalVolume* logicFirstMonitorLayer1 = new G4LogicalVolume(solidFirstMonitorLayer1,
                                                                   layer1MonitorChamberMaterial,
                                                                   "FirstMonitorLayer1");

    physiFirstMonitorLayer1 = new G4PVPlacement(0,
                                                G4ThreeVector(monitor1XPosition,0.*cm,0.*cm),
                                                "FirstMonitorLayer1",
                                                logicFirstMonitorLayer1,
                                                physicalTreatmentRoom, false, 0);

    G4Box* solidFirstMonitorLayer2 = new G4Box("FirstMonitorLayer2",
                                               monitor2XSize/2,
                                               monitorYSize/2,
                                               monitorZSize/2);

    G4LogicalVolume* logicFirstMonitorLayer2 = new G4LogicalVolume(solidFirstMonitorLayer2,
                                                                   layer2MonitorChamberMaterial,
                                                                   "FirstMonitorLayer2");

    physiFirstMonitorLayer2 = new G4PVPlacement(0, G4ThreeVector(monitor2XPosition,0.*cm,0.*cm),
                                                "FirstMonitorLayer2",
                                                logicFirstMonitorLayer2,
                                                physicalTreatmentRoom, false, 0);

    G4Box* solidFirstMonitorLayer3 = new G4Box("FirstMonitorLayer3",
                                               monitor3XSize/2,
                                               monitorYSize/2,
                                               monitorZSize/2);

    G4LogicalVolume* logicFirstMonitorLayer3 = new G4LogicalVolume(solidFirstMonitorLayer3,
                                                                   layer3MonitorChamberMaterial,
                                                                   "FirstMonitorLayer3");

    physiFirstMonitorLayer3 = new G4PVPlacement(0, G4ThreeVector(monitor3XPosition,0.*cm,0.*cm),                 //Hadrontherapy Example Zero point (origin) is at the 1st monitor chamber!!!!
                                                "FirstMonitorLayer3",
                                                logicFirstMonitorLayer3,
                                                physicalTreatmentRoom, false, 0);

    G4Box* solidFirstMonitorLayer4 = new G4Box("FirstMonitorLayer4",
                                               monitor4XSize/2,
                                               monitorYSize/2,
                                               monitorZSize/2);

    G4LogicalVolume* logicFirstMonitorLayer4 = new G4LogicalVolume(solidFirstMonitorLayer4,
                                                                   layer4MonitorChamberMaterial,
                                                                   "FirstMonitorLayer4");

    physiFirstMonitorLayer4 = new G4PVPlacement(0, G4ThreeVector(monitor4XPosition,0.*cm,0.*cm),
                                                "FirstMonitorLayer4",
                                                logicFirstMonitorLayer4,
                                                physicalTreatmentRoom, false, 0);

    G4Box* solidFirstMonitorLayer5 = new G4Box("FirstMonitorLayer5",
                                                monitor5XSize/2,
                                                monitorYSize/2,
                                                monitorZSize/2);

    G4LogicalVolume* logicFirstMonitorLayer5 = new G4LogicalVolume(solidFirstMonitorLayer5,
                                                                   layer5MonitorChamberMaterial,
                                                                    "FirstMonitorLayer5");

    physiFirstMonitorLayer5 = new G4PVPlacement(0 ,G4ThreeVector(monitor5XPosition, 0 *cm, 0 *cm),
                                                "FirstMonitorLayer5",
                                                logicFirstMonitorLayer5,
                                                physicalTreatmentRoom, false, 0);

    // ----------------------------//
    // THE SUB DOSE MONITOR CHAMBER  //
    // ----------------------------//
    physiSecondMonitorLayer1 = new G4PVPlacement(0, G4ThreeVector(monitor6XPosition *mm,0.*cm,0.*cm), "SecondMonitorLayer1",
                                                logicFirstMonitorLayer1,physicalTreatmentRoom, false, 0);

    physiSecondMonitorLayer2 = new G4PVPlacement(0, G4ThreeVector(monitor7XPosition,0.*cm,0.*cm), "SecondMonitorLayer2",
                                                 logicFirstMonitorLayer2, physicalTreatmentRoom, false, 0);

    physiSecondMonitorLayer3 = new G4PVPlacement(0, G4ThreeVector(monitor8XPosition,0.*cm,0.*cm), "MonitorLayer3",
                                                 logicFirstMonitorLayer3, physicalTreatmentRoom, false, 0);

    physiSecondMonitorLayer4 = new G4PVPlacement(0, G4ThreeVector(monitor9XPosition,0.*cm,0.*cm), "SecondMonitorLayer4",
                                                 logicFirstMonitorLayer4, physicalTreatmentRoom, false, 0);

    physiSecondMonitorLayer5 = new G4PVPlacement(0, G4ThreeVector(monitor10XPosition,0.*cm,0.*cm), "SecondMonitorLayer5",
                                                 logicFirstMonitorLayer5, physicalTreatmentRoom, false, 0);

    logicFirstMonitorLayer3 -> SetVisAttributes(blue);

}
/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::ProtontherapySpotPositionMonitorDetector()
{
    // -------------------------------------------------//
    //        THE SPOT POSITION MONITOR DETECTOR        //
    // -------------------------------------------------//
    // SPOT POSITION MONITOR DETECTOR:
    // This is the detector for detecting where the scanning beam
    // will hit.
    //
    // In this case it is simulated as:
    // 1. First kapton layer (window): 12.5 um,
    // 2. First coppr layer (window coating): 0.2 um,
    // 3. First aluminum layer (window coating): 0.1 um
    // 4. Second kapton layer (electrode): 12.5 um,
    // 5. Second copper layer (electrode): 0.2 um
    // 6. Second aluminum layer (electrode) : 0.1 um
    // 7. Agron gas layer for ionisation : 18 mm
    // Color used in the graphical output

    // First Kapton layer
    solidSpotPositionMonitorFirstKaptonLayer = new G4Box("SpotPositionMonitorFirstKaptonLayer",
                                          SpotPositionMonitorFirstKaptonLayerXSize/2,
                                          defaultSpotPositionMonitorYSize/2,
                                          defaultSpotPositionMonitorZSize/2);

    logicSpotPositionMonitorFirstKaptonLayer = new G4LogicalVolume(solidSpotPositionMonitorFirstKaptonLayer,
                                                    SpotPositionMonitorFirstKaptonLayerMaterial,
                                                    "SpotPositionMonitorFirstKaptonLayer");

    physiSpotPositionMonitorFirstKaptonLayer = new G4PVPlacement(0,
                                                  G4ThreeVector(SpotPositionMonitorFirstKaptonLayerXPosition,
                                                                0.0 ,
                                                                0.0),
                                                  "SpotPositionMonitorFirstKaptonLayer",
                                                  logicSpotPositionMonitorFirstKaptonLayer,
                                                  physicalTreatmentRoom,
                                                  false, 0);

    //First Copper Layer
    solidSpotPositionMonitorFirstCopperLayer = new G4Box("SpotPositionMonitorFirstCopperLayer",
                                          SpotPositionMonitorFirstCopperLayerXSize/2,
                                          defaultSpotPositionMonitorYSize/2,
                                          defaultSpotPositionMonitorZSize/2);

    logicSpotPositionMonitorFirstCopperLayer = new G4LogicalVolume(solidSpotPositionMonitorFirstCopperLayer,
                                                    SpotPositionMonitorFirstCopperLayerMaterial,
                                                    "SpotPositionMonitorFirstKaptonLayer");

    physiSpotPositionMonitorFirstCopperLayer = new G4PVPlacement(0, G4ThreeVector(SpotPositionMonitorFirstCopperLayerXPosition,
                                                                                  0.0,
                                                                                  0.0),
                                                   "SpotPositionMonitorFirstCopperLayer",
                                                   logicSpotPositionMonitorFirstCopperLayer,
                                                   physicalTreatmentRoom,
                                                   false, 0);


    // First Aluminum layer
    solidSpotPositionMonitorFirstAluminumLayer = new G4Box("SpotPositionMonitorFirstAluminumLayer",
                                            SpotPositionMonitorFirstAluminumLayerXSize/2,
                                            defaultSpotPositionMonitorYSize/2 ,
                                            defaultSpotPositionMonitorZSize/2);

    logicSpotPositionMonitorFirstAluminumLayer = new G4LogicalVolume(solidSpotPositionMonitorFirstAluminumLayer,
                                                      SpotPositionMonitorFirstAluminumLayerMaterial,
                                                      "SpotPositionMonitorFirstAluminumLayer");

    physiSpotPositionMonitorFirstAluminumLayer = new G4PVPlacement(0,
                                                    G4ThreeVector(SpotPositionMonitorFirstAluminumLayerXPosition,
                                                                  0.0 ,
                                                                  0.0),
                                                    "SpotPositionMonitorFirstAluminumLayer",
                                                    logicSpotPositionMonitorFirstAluminumLayer, physicalTreatmentRoom,
                                                    false, 0);

    // Second Kapton layer
    solidSpotPositionMonitorSecondKaptonLayer = new G4Box("SpotPositionMonitorSecondKaptonLayer",
                                           SpotPositionMonitorSecondKaptonLayerXSize/2,
                                           defaultSpotPositionMonitorYSize/2 ,
                                           defaultSpotPositionMonitorZSize/2);

    logicSpotPositionMonitorSecondKaptonLayer = new G4LogicalVolume(solidSpotPositionMonitorSecondKaptonLayer,
                                                     SpotPositionMonitorFirstKaptonLayerMaterial,
                                                     "SpotPositionMonitorSecondKaptonLayer");

    physiSpotPositionMonitorSecondKaptonLayer = new G4PVPlacement(0,
                                                   G4ThreeVector(SpotPositionMonitorSecondKaptonLayerXPosition,
                                                                 0.0,
                                                                 0.0),
                                                   "MOPISecondKaptonLayer",
                                                   logicSpotPositionMonitorSecondKaptonLayer,
                                                   physicalTreatmentRoom,
                                                   false, 0);



    // Second Aluminum layer
    solidSpotPositionMonitorSecondAluminumLayer = new G4Box("MOPISecondAluminumLayer",
                                             SpotPositionMonitorSecondAluminumLayerXSize/2,
                                             defaultSpotPositionMonitorYSize/2 ,
                                             defaultSpotPositionMonitorZSize/2);

    logicSpotPositionMonitorSecondAluminumLayer = new G4LogicalVolume(solidSpotPositionMonitorSecondAluminumLayer,
                                                       SpotPositionMonitorSecondAluminumLayerMaterial,
                                                       "SpotPositionMonitorSecondAluminumLayer");

    physiSpotPositionMonitorSecondAluminumLayer = new G4PVPlacement(0,
                                                     G4ThreeVector(SpotPositionMonitorSecondAluminumLayerXPosition,
                                                                   0.0 ,
                                                                   0.0),
                                                     "SpotPositionMonitorSecondAluminumLayer",
                                                     logicSpotPositionMonitorSecondAluminumLayer,
                                                     physicalTreatmentRoom,
                                                     false, 0);

    // Argon Gas Layer
    solidSpotPositionMonitorFirstArgonLayer = new G4Box("SpotPositionMonitorFirstArgonLayer",
                                      SpotPositionMonitorFirstArgonLayerXSize/2,
                                      defaultSpotPositionMonitorYSize/2,
                                      defaultSpotPositionMonitorZSize/2);

    logicSpotPositionMonitorFirstArgonLayer = new G4LogicalVolume(solidSpotPositionMonitorFirstArgonLayer,
                                                SpotPositionMonitorFirstArgonLayerMaterial,
                                                "SpotPositionMonitorFirstArgonLayer");

    physiSpotPositionMonitorFirstArgonLayer = new G4PVPlacement(0,
                                              G4ThreeVector(SpotPositionMonitorFirstArgonLayerXPosition,
                                                            0.0 ,
                                                            0.0),
                                              "MOPISecondAirGap",
                                              logicSpotPositionMonitorFirstArgonLayer, physicalTreatmentRoom,
                                              false, 0);


    logicSpotPositionMonitorFirstArgonLayer -> SetVisAttributes(darkGreen);

}
/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::ProtontherapyRangeShifter()
{
    // ---------------------------- //
    //         THE RANGE SHIFTER    //
    // -----------------------------//
    // It is a slab of ABS acting as energy degreader of
    // primary beam

    // RANGE SHIFTER: is a slab of ABS resin acting as energy degrader of
    // primary beam

    //Default material of the range shifter

    G4double defaultRangeShifterXSize = 20 *mm;
    rangeShifterXSize = defaultRangeShifterXSize;

    G4double defaultRangeShifterYSize = 10 *cm;
    rangeShifterYSize = defaultRangeShifterYSize;

    G4double defaultRangeShifterZSize = 10 *cm;
    rangeShifterZSize = defaultRangeShifterZSize;

    G4double defaultRangeShifterXPosition = -425 *mm - defaultRangeShifterXSize/2;
    rangeShifterXPosition = defaultRangeShifterXPosition;

    G4double defaultRangeShifterYPosition = 0.0 *mm;
    rangeShifterYPosition = defaultRangeShifterYPosition;

    G4double defaultRangeShifterZPosition = 0.0 *mm;
    rangeShifterZPosition = defaultRangeShifterZPosition;

    solidRangeShifterBox = new G4Box("RangeShifterBox",
                                     rangeShifterXSize/2,
                                     rangeShifterYSize/2,
                                     rangeShifterZSize/2);

    logicRangeShifterBox = new G4LogicalVolume(solidRangeShifterBox,
                                               rangeShifterMaterial,
                                               "RangeShifterBox");
    physiRangeShifterBox = new G4PVPlacement(0,
                                             G4ThreeVector(rangeShifterXPosition, 0., 0.),
                                             "RangeShifterBox",
                                             logicRangeShifterBox,
                                             physicalTreatmentRoom,
                                             false,
                                             0);


    logicRangeShifterBox -> SetVisAttributes(yellow);
}

void ScanningProtonBeamLine::ProtontherapyNozzleBoundary() {

    G4double centerPos = -345.0*mm - 600.0/2*mm;
    G4RotationMatrix* rm1 = new G4RotationMatrix;
    rm1->rotateY(90.0*deg);

    G4Cons* solidNozzleBoundary = new G4Cons("NozzleBoundary",
                                        8*cm, 8.5*cm,
                                        25*cm, 25.5*cm,
                                        600*mm/2,
                                        0.0, 360*deg);
    G4LogicalVolume* logicNozzleBoundary = new G4LogicalVolume(solidNozzleBoundary,
                                                               vacuumZoneMaterial,
                                                               "NozzleBoundary");
    G4PVPlacement* physiNozzleBoundary = new G4PVPlacement(rm1,
                                                          G4ThreeVector(centerPos,0,0),
                                                          "NozzleBoundary",
                                                          logicNozzleBoundary,
                                                          physicalTreatmentRoom,
                                                          false,
                                                          0);
    logicNozzleBoundary->SetVisAttributes(gray);
}

/////////////////////////// MESSENGER ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::SetRangeShifterXPosition(G4double value)
{
    physiRangeShifterBox -> SetTranslation(G4ThreeVector(value, 0., 0.));
    G4RunManager::GetRunManager() -> GeometryHasBeenModified();
    G4cout << "The Range Shifter is translated to"<< value/mm <<"mm along the X axis" <<G4endl;
}
/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::SetRangeShifterXSize(G4double value)
{
    solidRangeShifterBox -> SetXHalfLength(value) ;
    G4cout << "RangeShifter size X (mm): "<< ((solidRangeShifterBox -> GetXHalfLength())*2.)/mm
    << G4endl;
    G4RunManager::GetRunManager() -> GeometryHasBeenModified();
}

/////////////////////////////////////////////////////////////////////////////
void ScanningProtonBeamLine::SetRSMaterial(G4String materialChoice)
{
    if (G4Material* pttoMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice, false) )
    {
        if (pttoMaterial)
        {
            rangeShifterMaterial  = pttoMaterial;
            logicRangeShifterBox -> SetMaterial(pttoMaterial);
            G4cout << "The material of the Range Shifter has been changed to " << materialChoice << G4endl;
        }
    }
    else
    {
        G4cout << "WARNING: material \"" << materialChoice << "\" doesn't exist in NIST elements/materials"
	    " table [located in $G4INSTALL/source/materials/src/G4NistMaterialBuilder.cc]" << G4endl;
        G4cout << "Use command \"/parameter/nist\" to see full materials list!" << G4endl;
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void ScanningProtonBeamLine::SetMagneticField(G4ThreeVector value)
{
  //create magnetic field
  G4UniformMagField* magField = new G4UniformMagField(value*tesla);
  G4FieldManager* fieldMgr = new G4FieldManager;
  G4Mag_UsualEqRhs* fEquation = new G4Mag_UsualEqRhs(magField);
  G4MagIntegratorStepper* fStepper ;
  fStepper = new G4HelixImplicitEuler( fEquation );

  fieldMgr->SetDetectorField(magField);
  G4MagInt_Driver* fIntgrDriver = new G4MagInt_Driver(1.*mm,
                                     fStepper);

  //G4ChordFinder*  localChordFinder = new G4ChordFinder(magField,0.5*mm);
  G4ChordFinder*  localChordFinder = new G4ChordFinder(fIntgrDriver);

  fieldMgr->SetChordFinder(localChordFinder);
  fieldMgr->GetChordFinder()->SetDeltaChord(0.5*mm);
  fieldMgr->SetDeltaOneStep(.5*mm);
  //fieldMgr->CreateChordFinder(magField);
  logicMagnetX->SetFieldManager(fieldMgr,true);
}

void ScanningProtonBeamLine::SetFieldAngle(G4ThreeVector rotVector) { 
    rotVector[0] = 0.; 
    fieldRotVector = rotVector;
}

void ScanningProtonBeamLine::SetDicomActivation(G4Bool option) { 
    
}

// This section is for scoring manager

#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSDoseDeposit3D.hh"
#include <set>
#include <vector>

void ScanningProtonBeamLine::ConstructSDandField()
{
  G4cout << "\t CONSTRUCT SD AND FIELD" << G4endl;

  // Sensitive Detector Name
  G4String concreteSDname = "phantomSD";
  std::vector<G4String> scorer_names;
  scorer_names.push_back(concreteSDname);
  //------------------------
  // MultiFunctionalDetector
  //------------------------
  //
  // Define MultiFunctionalDetector with name.
  // declare MFDet as a MultiFunctionalDetector scorer
  G4MultiFunctionalDetector* MFDet =
    new G4MultiFunctionalDetector(concreteSDname);
  G4SDManager::GetSDMpointer()->AddNewDetector( MFDet );

  G4int fNVoxelX = protontherapyDetectorConstruction->fNVoxelX;
  G4int fNVoxelY = protontherapyDetectorConstruction->fNVoxelY;
  G4int fNVoxelZ = protontherapyDetectorConstruction->fNVoxelZ;

  G4cout<<fNVoxelX<<" "<<fNVoxelY<<" "<<fNVoxelZ<<G4endl;

  G4VPrimitiveScorer* dosedep = new G4PSDoseDeposit3D("DoseDeposit", fNVoxelX, fNVoxelY, fNVoxelZ);
  MFDet->RegisterPrimitive(dosedep);

  fScorers = protontherapyDetectorConstruction->GetScorerLogicalVolume();
  for(std::set<G4LogicalVolume*>::iterator ite = fScorers.begin(); ite != fScorers.end(); ++ite) {
    SetSensitiveDetector(*ite, MFDet);
  }
}

