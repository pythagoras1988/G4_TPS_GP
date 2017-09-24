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

#include "ProtontherapyActionInitialization.hh"
#include "ProtontherapyPrimaryGeneratorAction.hh"
#include "ProtontherapyRunAction.hh"
#include "ProtontherapySteppingAction.hh"
#include "ProtontherapyDetectorConstruction.hh"
#include "G4GeneralParticleSource.hh"

#include "ProtontherapyEventAction.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ProtontherapyActionInitialization::ProtontherapyActionInitialization(/*G4VUserDetectorConstruction* detConstruction*/)
: G4VUserActionInitialization()//,
 // fDetectorConstruction(detConstruction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ProtontherapyActionInitialization::~ProtontherapyActionInitialization()
{/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ProtontherapyActionInitialization::BuildForMaster() const
{
	// In MT mode, to be clearer, the RunAction class for the master thread might be
	// different than the one used for the workers.
	// This RunAction will be called before and after starting the
	// workers.
	// For more details, please refer to :
	// https://twiki.cern.ch/twiki/bin/view/Geant4/Geant4MTForApplicationDevelopers
	//
	// HadrontherapyRunAction* runAction= new HadrontherapyRunAction(fDetectorConstruction);
	// SetUserAction(runAction);
  ProtontherapyRunAction* pRunAction = new ProtontherapyRunAction();
  SetUserAction(pRunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ProtontherapyActionInitialization::Build() const
{       
 // Initialize the primary particles
  ProtontherapyPrimaryGeneratorAction *pPrimaryGenerator = new ProtontherapyPrimaryGeneratorAction();
  SetUserAction(pPrimaryGenerator);
	
  // Optional UserActions: run, event, stepping
  ProtontherapyRunAction* pRunAction = new ProtontherapyRunAction();
  SetUserAction(pRunAction);

	
  ProtontherapyEventAction* pEventAction = new ProtontherapyEventAction();
  SetUserAction(pEventAction);
	
  ProtontherapySteppingAction* steppingAction = new ProtontherapySteppingAction(pRunAction); 
  SetUserAction(steppingAction);  


       
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
