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

#include "ProtontherapyStepMax.hh"
#include "ProtontherapyStepMaxMessenger.hh"
#include "G4Track.hh"

/////////////////////////////////////////////////////////////////////////////
ProtontherapyStepMax::ProtontherapyStepMax(const G4String& processName)
 : G4VDiscreteProcess(processName),MaxChargedStep(DBL_MAX)
{
  //Activate messenger for stepMax
  pMess = new ProtontherapyStepMaxMessenger(this);
}

/////////////////////////////////////////////////////////////////////////////
ProtontherapyStepMax::~ProtontherapyStepMax() { delete pMess; }

/////////////////////////////////////////////////////////////////////////////
G4bool ProtontherapyStepMax::IsApplicable(const G4ParticleDefinition& particle)
{
  return (particle.GetPDGCharge() != 0.);
}

/////////////////////////////////////////////////////////////////////////////
void ProtontherapyStepMax::SetMaxStep(G4double step) {MaxChargedStep = step;}

/////////////////////////////////////////////////////////////////////////////
G4double ProtontherapyStepMax::PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
                                                  G4double,
                                                  G4ForceCondition* condition )
{
  // condition is set to "Not Forced"
  *condition = NotForced;

  G4double ProposedStep = DBL_MAX;
if((MaxChargedStep > 0.) &&
   (aTrack.GetVolume() != 0) && ((aTrack.GetVolume()->GetName() == "phantom")||(aTrack.GetVolume()->GetName() == "InternalChamber")||(aTrack.GetVolume()->GetName() == "CollimatorHole")||(aTrack.GetVolume()->GetName() == "PhysFourthTQuad")||(aTrack.GetVolume()->GetName() == "PhysThirdTQuad")||(aTrack.GetVolume()->GetName() == "PhysSecondTQuad")||(aTrack.GetVolume()->GetName() == "PhysFirstTQuad")||(aTrack.GetVolume()->GetName() =="physQuadChamber")||(aTrack.GetVolume()->GetName() =="PVirtualMag")||(aTrack.GetVolume()->GetName() =="PhysicCup")))

     ProposedStep = MaxChargedStep;

  return ProposedStep;
}

/////////////////////////////////////////////////////////////////////////////
G4VParticleChange* ProtontherapyStepMax::PostStepDoIt(const G4Track& aTrack, const G4Step&)
{
   // do nothing
   aParticleChange.Initialize(aTrack);
   return &aParticleChange;
}
