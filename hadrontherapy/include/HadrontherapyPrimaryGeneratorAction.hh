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

#ifndef HadrontherapyPrimaryGeneratorAction_h
#define HadrontherapyPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "HadrontherapyPrimaryGeneratorMessenger.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;

class HadrontherapyPrimaryGeneratorMessenger;
class HadrontherapyPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  HadrontherapyPrimaryGeneratorAction();
  ~HadrontherapyPrimaryGeneratorAction();

public:
  // Methods to change the parameters of primary particle generation
  // interactively
  void GeneratePrimaries(G4Event*);
  void SetTwissAlpha(G4double);
  void SetTwissBeta(G4double);
  void SetTwissEmittance_y(G4double);
  void SetTwissEmittance_z(G4double);

private:
  HadrontherapyPrimaryGeneratorMessenger* messenger;
  G4double epsilon_y, epsilon_z;
  G4double alpha, beta, gamma;
  G4double xPos, y_sample, z_sample;
  G4double yp_sample, zp_sample;
  G4double dc_y, dc_z, dc_x;
  G4double var_y, var_yp, var_z, var_zp, cov_y, cov_z;

private:
  //G4GeneralParticleSource* particleGun;
  G4ParticleGun* particleGun;
  G4double sigmaX;
};

#endif
