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

#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4VVisManager.hh"

#include "ProtontherapyEventAction.hh"
#include "ProtontherapyEventActionMessenger.hh"

/////////////////////////////////////////////////////////////////////////////
ProtontherapyEventAction::ProtontherapyEventAction() :
  drawFlag("all" ),printModulo(10), pointerEventMessenger(0)
{
  pointerEventMessenger = new ProtontherapyEventActionMessenger(this);
}

/////////////////////////////////////////////////////////////////////////////
ProtontherapyEventAction::~ProtontherapyEventAction()
{
 delete pointerEventMessenger;
}

/////////////////////////////////////////////////////////////////////////////
void ProtontherapyEventAction::BeginOfEventAction(const G4Event* evt)
{
  G4int evtNb = evt->GetEventID();
  //printing survey
  if (evtNb%printModulo == 0)
     G4cout << "\n---> Begin of Event: " << evtNb << G4endl;

}

/////////////////////////////////////////////////////////////////////////////
void ProtontherapyEventAction::EndOfEventAction(const G4Event* evt)
{
}
