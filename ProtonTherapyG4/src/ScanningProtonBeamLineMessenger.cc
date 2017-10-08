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

#include "ScanningProtonBeamLineMessenger.hh"
#include "ScanningProtonBeamLine.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4SystemOfUnits.hh"

ScanningProtonBeamLineMessenger::ScanningProtonBeamLineMessenger(ScanningProtonBeamLine* beamLine)
:scanningProton(beamLine)

{
    changeTheBeamLineDir = new G4UIdirectory("/ChangeBeamLine/");
    changeTheBeamLineDir -> SetGuidance("Command to change the transport beam line");

    changeTheBeamLineNameCmd = new G4UIcmdWithAString("/ChangeBeamLine/beamLineName",this);
    changeTheBeamLineNameCmd -> SetGuidance("Insert the name of the beam line you want simulate");
    changeTheBeamLineNameCmd -> SetParameterName("List",false);
    changeTheBeamLineNameCmd -> AvailableForStates(G4State_PreInit);

    beamLineDir = new G4UIdirectory("/beamLine/");
    beamLineDir -> SetGuidance("set specification of range shifter");

    rangeShifterDir = new G4UIdirectory("/beamLine/RangeShifter/");
    rangeShifterDir -> SetGuidance("set specification of range shifter");

    rangeShifterMatCmd = new G4UIcmdWithAString("/beamLine/RangeShifter/RSMat",this);
    rangeShifterMatCmd -> SetGuidance("Set material of range shifter");
    rangeShifterMatCmd -> SetParameterName("choice",false);
    rangeShifterMatCmd -> AvailableForStates(G4State_Idle);

    rangeShifterXSizeCmd = new G4UIcmdWithADoubleAndUnit("/beamLine/RangeShifter/thickness",this);
    rangeShifterXSizeCmd -> SetGuidance("Set half of the thickness of range shifter along X axis");
    rangeShifterXSizeCmd -> SetParameterName("Size",false);
    rangeShifterXSizeCmd -> SetDefaultUnit("mm");
    rangeShifterXSizeCmd -> SetUnitCandidates("mm cm m");
    rangeShifterXSizeCmd -> AvailableForStates(G4State_Idle);

    rangeShifterXPositionCmd = new G4UIcmdWithADoubleAndUnit("/beamLine/RangeShifter/position",this);
    rangeShifterXPositionCmd -> SetGuidance("Set position of range shifter");
    rangeShifterXPositionCmd -> SetParameterName("Size",false);
    rangeShifterXPositionCmd -> SetDefaultUnit("mm");
    rangeShifterXPositionCmd -> SetUnitCandidates("mm cm m");
    rangeShifterXPositionCmd -> AvailableForStates(G4State_Idle);

    magneticFieldCmd = new G4UIcmdWith3Vector("/beamLine/setField",this);
    magneticFieldCmd -> SetGuidance("Set Magnetic field in the X Y Z direction");
    magneticFieldCmd -> SetParameterName("field X", "field Y", "field Z", false);
    magneticFieldCmd -> AvailableForStates(G4State_Idle);

}

ScanningProtonBeamLineMessenger::~ScanningProtonBeamLineMessenger()
{

    delete rangeShifterXPositionCmd;
    delete rangeShifterXSizeCmd;
    delete rangeShifterMatCmd;
    delete magneticFieldCmd;
    delete rangeShifterDir;
    delete beamLineDir;
}

void ScanningProtonBeamLineMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

    if( command == rangeShifterMatCmd )
    { scanningProton -> SetRSMaterial(newValue);}

    else if( command == rangeShifterXSizeCmd )
    { scanningProton -> SetRangeShifterXSize
	(rangeShifterXSizeCmd -> GetNewDoubleValue(newValue));}

    else if( command == rangeShifterXPositionCmd )
    { scanningProton -> SetRangeShifterXPosition
	(rangeShifterXPositionCmd -> GetNewDoubleValue(newValue));}

    else if( command == magneticFieldCmd) {
      scanningProton -> SetMagneticField(magneticFieldCmd->GetNew3VectorValue(newValue));
    }

}
