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

#include "ProtontherapyPrimaryGeneratorMessenger.hh"
#include "ProtontherapyPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4SystemOfUnits.hh"

ProtontherapyPrimaryGeneratorMessenger::ProtontherapyPrimaryGeneratorMessenger(
                                             ProtontherapyPrimaryGeneratorAction* ProtontherapyGun)
:ProtontherapyAction(ProtontherapyGun)
{
  //
  // Definition of the interactive commands to modify the parameters of the
  // generation of primary particles
  //
 beamParametersDir = new G4UIdirectory("/beam/");
 beamParametersDir -> SetGuidance("set parameters of beam");

 EnergyDir = new G4UIdirectory("/beam/energy/");
 EnergyDir -> SetGuidance ("set energy of beam");

 particlePositionDir = new G4UIdirectory("/beam/position/");
 particlePositionDir -> SetGuidance ("set position of particle");

 MomentumDir = new G4UIdirectory("/beam/momentum/");
 MomentumDir -> SetGuidance ("set momentum of particle ");

 twissDir = new G4UIdirectory("/twiss/");
 twissDir -> SetGuidance("Set the twiss parameters of the beam");

 twissAlphaCmd = new G4UIcmdWithADouble("/twiss/alpha",this);
 twissAlphaCmd -> SetParameterName("Alpha Twiss parameter",false);

 twissBetaCmd = new G4UIcmdWithADouble("/twiss/beta",this);
 twissBetaCmd -> SetParameterName("Beta Twiss parameter",false);

 twissYEmittanceCmd = new G4UIcmdWithADouble("/twiss/emittance_y",this);
 twissYEmittanceCmd -> SetParameterName("Emittance_y Twiss parameter",false);

 twissZEmittanceCmd = new G4UIcmdWithADouble("/twiss/emittance_z",this);
 twissZEmittanceCmd -> SetParameterName("Emittance_z Twiss parameter",false);

}

ProtontherapyPrimaryGeneratorMessenger::~ProtontherapyPrimaryGeneratorMessenger()
{
  delete beamParametersDir;
  delete EnergyDir;
  delete particlePositionDir;
  delete MomentumDir;
  delete twissDir;
  delete twissAlphaCmd;
  delete twissBetaCmd;
  delete twissYEmittanceCmd;
  delete twissZEmittanceCmd;
}

void ProtontherapyPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if ( command == twissAlphaCmd )
    { ProtontherapyAction -> SetTwissAlpha(twissAlphaCmd
						  -> GetNewDoubleValue(newValue));}
  if ( command == twissBetaCmd )
    { ProtontherapyAction -> SetTwissBeta(twissBetaCmd
						  -> GetNewDoubleValue(newValue));}
  if ( command == twissYEmittanceCmd )
    { ProtontherapyAction -> SetTwissEmittance_y(twissYEmittanceCmd
						  -> GetNewDoubleValue(newValue));}
  if ( command == twissZEmittanceCmd )
    { ProtontherapyAction -> SetTwissEmittance_z(twissZEmittanceCmd
						  -> GetNewDoubleValue(newValue));}
}
